/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "rmxmedia.h"


namespace rmx
{

	AudioManager::AudioManager() :
		mAudioDeviceID(0),
		mAudioLocks(0),
		mNextFreeID(1),
		mChangeCounter(0),
		mPlayedSamples(0),
		mTimeSinceLastUpdate(0.0f),
		mRootMixer(*new AudioMixer(0))	// Root mixer always uses ID 0
	{
		mAudioMixers[0] = &mRootMixer;
		memset(&mFormat, 0, sizeof(mFormat));
	}

	AudioManager::~AudioManager()
	{
		RMX_ASSERT(mAudioLocks == 0, "Pending audio locks");

		// Delete all audio mixers (including the root mixer)
		for (std::map<int, AudioMixer*>::iterator it = mAudioMixers.begin(); it != mAudioMixers.end(); ++it)
		{
			delete it->second;
		}
	}

	void AudioManager::initialize(int sample_freq, int channels, int audioBufferSamples)
	{
		if (!FTX::System->initialize())
			return;

		// Reset instances
		mInstances.clear();
		mRootMixer.clearAudioInstances();

		// Open audio device
		SDL_AudioSpec desired;
		memset(&desired, 0, sizeof(desired));
		desired.freq = sample_freq;
		desired.format = AUDIO_S16LSB;
		desired.channels = (uint8)channels;
		desired.samples = (uint16)audioBufferSamples;
		desired.callback = mixAudioStatic;
		desired.userdata = this;

#ifdef PLATFORM_WEB
		mAudioDeviceID = SDL_OpenAudioDevice(nullptr, 0, &desired, &mFormat, 0);
#else
		mAudioDeviceID = SDL_OpenAudioDevice(nullptr, 0, &desired, &mFormat, SDL_AUDIO_ALLOW_ANY_CHANGE);
#endif
		if (mAudioDeviceID == 0)
		{
			RMX_ERROR("Failed to open audio device: " << SDL_GetError(), return);
		}

		// Success!
		playAudio(true);
	}

	void AudioManager::exit()
	{
		if (mAudioDeviceID != 0)
		{
			SDL_CloseAudioDevice(mAudioDeviceID);
			mAudioDeviceID = 0;
		}
	}

	void AudioManager::clear()
	{
		if (!mInstances.empty())
		{
			lockAudio();
			for (std::map<int, AudioMixer*>::iterator it = mAudioMixers.begin(); it != mAudioMixers.end(); ++it)
			{
				it->second->clearAudioInstances();
			}
			unlockAudio();

			mInstances.clear();
			mRemoveIDs.clear();
			++mChangeCounter;
		}
	}

	void AudioManager::playAudio(bool onoff)
	{
		SDL_PauseAudioDevice(mAudioDeviceID, onoff ? 0 : 1);
	}

	bool AudioManager::getAudioState()
	{
		return (SDL_GetAudioStatus() == SDL_AUDIO_PLAYING);
	}

	void AudioManager::lockAudio()
	{
		if (mAudioLocks == 0)
		{
			SDL_LockAudioDevice(mAudioDeviceID);
		}
		++mAudioLocks;
	}

	void AudioManager::unlockAudio()
	{
		RMX_ASSERT(mAudioLocks > 0, "Called 'AudioManager::unlockAudio' without locking");
		--mAudioLocks;
		if (mAudioLocks == 0)
		{
			SDL_UnlockAudioDevice(mAudioDeviceID);
		}
	}

	void AudioManager::regularUpdate(float timeElapsed)
	{
		// Do the following cleanup only every 0.5 seconds
		mTimeSinceLastUpdate += timeElapsed;
		if (mTimeSinceLastUpdate >= 0.5f)
		{
			mTimeSinceLastUpdate = 0.0f;
			lockAudio();

			// Collect non-persistent audio buffers currently played back, and the earliest playback positions
			static std::vector<std::pair<AudioBuffer*, int>> audioBufferPurgePositions;
			audioBufferPurgePositions.clear();
			for (std::map<int, AudioInstance>::iterator it_inst = mInstances.begin(); it_inst != mInstances.end(); ++it_inst)
			{
				const AudioInstance& instance = it_inst->second;
				AudioBuffer* audioBuffer = instance.mAudioBuffer;
				if (nullptr != audioBuffer && !audioBuffer->isPersistent())
				{
					// Already had this one before?
					bool found = false;
					for (size_t i_bp = 0; i_bp < audioBufferPurgePositions.size(); ++i_bp)
					{
						if (audioBufferPurgePositions[i_bp].first == audioBuffer)
						{
							// Update the playback position
							audioBufferPurgePositions[i_bp].second = std::min(audioBufferPurgePositions[i_bp].second, instance.mPosition);
							found = true;
							break;
						}
					}

					if (!found)
					{
						audioBufferPurgePositions.push_back(std::make_pair(audioBuffer, instance.mPosition));
					}
				}
			}

			// Now update the audio buffers
			for (size_t i_bp = 0; i_bp < audioBufferPurgePositions.size(); ++i_bp)
			{
				AudioBuffer* audioBuffer = audioBufferPurgePositions[i_bp].first;
				audioBuffer->lock();
				audioBuffer->markPurgeableSamples(audioBufferPurgePositions[i_bp].second);
				audioBuffer->unlock();
			}

			unlockAudio();
		}
	}

	void AudioManager::setGlobalVolume(float volume)
	{
		mRootMixer.setVolume(volume);
	}

	AudioMixer* AudioManager::getAudioMixerByID(int mixerId) const
	{
		const std::map<int, AudioMixer*>::const_iterator it = mAudioMixers.find(mixerId);
		return (it == mAudioMixers.end()) ? nullptr : it->second;
	}

	void AudioManager::deleteAudioMixerByID(int mixerId)
	{
		RMX_ASSERT(mixerId != 0, "Can't delete root audio mixer (with ID 0)");
		mAudioMixers.erase(mixerId);
	}

	float AudioManager::getAudioMixerVolumeByID(int mixerId) const
	{
		const AudioMixer* audioMixer = getAudioMixerByID(mixerId);
		return (nullptr != audioMixer) ? audioMixer->getVolume() : 0.0f;
	}

	void AudioManager::setAudioMixerVolumeByID(int mixerId, float relativeVolume)
	{
		AudioMixer* audioMixer = getAudioMixerByID(mixerId);
		if (nullptr != audioMixer)
		{
			audioMixer->setVolume(relativeVolume);
		}
	}

	bool AudioManager::addSound(const PlaybackOptions& playbackOptions, AudioReference& ref)
	{
		if (nullptr == playbackOptions.mAudioBuffer)
			return false;

		AudioMixer* audioMixer = getAudioMixerByID(playbackOptions.mAudioMixerId);
		if (nullptr == audioMixer)
			return false;

		AudioInstance& instance = mInstances[mNextFreeID];
		instance.mID = mNextFreeID;
		instance.mAudioBuffer = playbackOptions.mAudioBuffer;
		instance.mAudioMixer = audioMixer;
		instance.mVolume = playbackOptions.mVolume;
		instance.mVolumeChange = playbackOptions.mVolumeChange;
		instance.mSpeed = playbackOptions.mSpeed;
		instance.mPosition = roundToInt(playbackOptions.mPosition * (float)playbackOptions.mAudioBuffer->getFrequency());
		instance.mLoop = playbackOptions.mLoop;
		instance.mStreaming = playbackOptions.mStreaming;

		lockAudio();
		audioMixer->addAudioInstance(instance);
		unlockAudio();

		++mChangeCounter;
		++mNextFreeID;

		ref.setInstanceID(instance.mID);
		return (instance.mID != 0);
	}

	int AudioManager::addSound(AudioBuffer* audiobuffer, bool streaming)
	{
		PlaybackOptions playbackOptions;
		playbackOptions.mAudioBuffer = audiobuffer;
		playbackOptions.mStreaming = streaming;

		AudioReference ref;
		addSound(playbackOptions, ref);
		return ref.getInstanceID();
	}

	bool AudioManager::addSound(AudioBuffer* audiobuffer, AudioReference& ref, bool streaming)
	{
		PlaybackOptions playbackOptions;
		playbackOptions.mAudioBuffer = audiobuffer;
		playbackOptions.mStreaming = streaming;
		return addSound(playbackOptions, ref);
	}

	void AudioManager::removeSound(AudioReference& ref)
	{
		if (ref.valid())
		{
			removeInstance(ref.getInstanceID());
		}
	}

	void AudioManager::removeAllSounds()
	{
		// This function is just a synonym for clear
		clear();
	}

	AudioManager::AudioInstance* AudioManager::findInstance(int ID)
	{
		if (ID <= 0 || ID >= mNextFreeID)
			return nullptr;

		// This seems like a good place to do some cleanup if needed
		processRemoveIDs();

		const std::map<int, AudioInstance>::iterator it = mInstances.find(ID);
		if (it == mInstances.end())
			return nullptr;

		return &it->second;
	}

	void AudioManager::removeInstance(int ID)
	{
		assert(ID > 0 && ID < mNextFreeID);
		const std::map<int, AudioInstance>::iterator it = mInstances.find(ID);
		if (it != mInstances.end())
		{
			AudioInstance& audioInstance = it->second;
			if (nullptr != audioInstance.mAudioMixer)
			{
				lockAudio();
				audioInstance.mAudioMixer->removeAudioInstance(audioInstance);
				unlockAudio();
			}

			mInstances.erase(it);
			++mChangeCounter;
		}
	}

	void AudioManager::processRemoveIDs()
	{
		if (!mRemoveIDs.empty())
		{
			lockAudio();
			for (size_t i = 0; i < mRemoveIDs.size(); ++i)
			{
				int ID = mRemoveIDs[i];
				const std::map<int, AudioInstance>::iterator it = mInstances.find(ID);
				if (it != mInstances.end())
				{
					AudioInstance& audioInstance = it->second;
					if (nullptr != audioInstance.mAudioMixer)
					{
						audioInstance.mAudioMixer->removeAudioInstance(audioInstance);
					}
				}
			}
			unlockAudio();

			for (size_t i = 0; i < mRemoveIDs.size(); ++i)
			{
				mInstances.erase(mRemoveIDs[i]);
			}
			mRemoveIDs.clear();
			++mChangeCounter;
		}
	}

	void AudioManager::registerAudioMixer(AudioMixer& audioMixer, int parentMixerId)
	{
		// Is there another audio mixer with the same ID already?
		const std::map<int, AudioMixer*>::iterator it = mAudioMixers.find(audioMixer.mMixerId);
		if (it != mAudioMixers.end() && it->second != &audioMixer)
		{
			// Replace the old mixer in the hierarchy
			AudioMixer* oldMixer = it->second;

			std::swap(audioMixer.mParent, oldMixer->mParent);
			for (size_t i = 0; i < audioMixer.mParent->mChildren.size(); ++i)
			{
				if (audioMixer.mParent->mChildren[i] == oldMixer)
					audioMixer.mParent->mChildren[i] = &audioMixer;
			}

			std::swap(audioMixer.mChildren, oldMixer->mChildren);
			for (size_t i = 0; i < audioMixer.mChildren.size(); ++i)
			{
				audioMixer.mChildren[i]->mParent = &audioMixer;
			}

			delete oldMixer;
		}

		// Register under its mixer ID
		mAudioMixers[audioMixer.mMixerId] = &audioMixer;

		// Register at (new) parent
		AudioMixer* parent = getAudioMixerByID(parentMixerId);
		if (nullptr == parent)
			parent = &mRootMixer;
		parent->addChild(audioMixer);
	}

	void AudioManager::mixAudioStatic(void* _userdata, uint8* outputStream, int outputBytes)
	{
		static_cast<AudioManager*>(_userdata)->mixAudio(outputStream, outputBytes);
	}

	void AudioManager::mixAudio(uint8* outputStream, int outputBytes)
	{
		const size_t outputSamples = outputBytes / (mFormat.channels * sizeof(short));
		const size_t MAX_SAMPLES = 2048;

		RMX_ASSERT(outputSamples <= MAX_SAMPLES, "Mixing more than " << MAX_SAMPLES << " samples at once is not supported");
		RMX_ASSERT(mFormat.channels <= 2, "More than 2 channels is not supported");

		// Setup intermediate buffer
		static int32 fullOutputBuffer[MAX_SAMPLES * 2];
		memset(fullOutputBuffer, 0, sizeof(fullOutputBuffer));

		AudioMixer::MixerParameters parameters;
		parameters.mOutputBuffers[0] = &fullOutputBuffer[0];
		parameters.mOutputBuffers[1] = &fullOutputBuffer[outputSamples];
		parameters.mOutputSamples = outputSamples;
		parameters.mOutputFormat = &mFormat;
		parameters.mAccumulatedVolume = 1.0f;
		mRootMixer.performAudioMix(parameters);

		// Copy results into the output stream
		for (int channelIndex = 0; channelIndex < mFormat.channels; ++channelIndex)
		{
			const int32* RESTRICT src = parameters.mOutputBuffers[channelIndex];
			short* dst = ((short*)outputStream) + channelIndex;

			for (size_t i = 0; i < outputSamples; ++i)
			{
				if (*src >= 0x800000)
				{
					*dst = 0x7fff;
				}
				else if (*src < -0x800000)
				{
					*dst = -0x8000;
				}
				else
				{
					*dst = (short)(*src >> 8);
				}
				++src;
				dst += 2;
			}
		}

		mPlayedSamples += (uint32)outputSamples;

		// Remove instance that are done playing
		for (std::map<int, AudioInstance>::iterator it = mInstances.begin(); it != mInstances.end(); ++it)
		{
			if (it->second.mPlaybackDone)
			{
				// Add to remove IDs list, but only once please
				if (!containsElement(mRemoveIDs, it->first))
				{
					mRemoveIDs.push_back(it->first);
				}
				++mChangeCounter;
			}
		}
	}



	bool WavLoader::load(AudioBuffer* buffer, const String& source, const String& params)
	{
		// Load WAV file
		if (nullptr == buffer || source.empty())
			return false;
		if (!source.endsWith(".wav"))
			return false;

		// Just use SDL for this
		SDL_AudioSpec wave;
		unsigned char* wave_data;
		unsigned int wave_length;
		if (!SDL_LoadWAV(*source, &wave, &wave_data, &wave_length))
			return false;

		// Rebuild audio buffer
		int frequency = 44100;
		int channels = 2;
		buffer->clear(frequency, channels);

		// Convert format
		SDL_AudioCVT cvt;
		SDL_BuildAudioCVT(&cvt, wave.format, wave.channels, wave.freq, AUDIO_S16LSB, channels, frequency);
		cvt.buf = new unsigned char[wave_length * cvt.len_mult];
		memcpy(cvt.buf, wave_data, wave_length);
		cvt.len = wave_length;
		SDL_ConvertAudio(&cvt);
		SDL_FreeWAV(wave_data);

		// Write data
		const short* data = (short*)cvt.buf;
		const int length = cvt.len_cvt / (channels * sizeof(short));
		short buf0[2048];
		short buf1[2048];
		short* buf[2] = { buf0, buf1 };
		const int pages = length/2048+1;
		for (int page = 0; page < pages; ++page)
		{
			int pagesize = (page < pages-1) ? 2048 : (length % 2048);
			for (int i = 0; i < pagesize; ++i)
				for (int j = 0; j < 2; ++j)
					buf[j][i] = data[(page*2048+i)*2+j];
			buffer->addData(buf, pagesize);
		}
		delete[] cvt.buf;
		return true;
	}

}
