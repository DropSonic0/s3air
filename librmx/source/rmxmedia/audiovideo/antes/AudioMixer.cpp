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

	namespace
	{
		void mixInSamples(int32* output, const short* input, int numSamples, int sourceIndexStart, int sourceIndexAdvance, int volume, int volumeChange)
		{
			int j = sourceIndexStart;
			if (volumeChange == 0)
			{
				volume >>= 8;
				for (int i = 0; i < numSamples; ++i)
				{
					output[i] += input[j >> 16] * volume;
					j += sourceIndexAdvance;
				}
			}
			else
			{
				if (volume + volumeChange * numSamples < 0)
				{
					numSamples = -volume / volumeChange;
				}
				else if (volume + volumeChange * numSamples > 0x10000)
				{
					numSamples = (0x10000 - volume) / volumeChange;
				}

				for (int i = 0; i < numSamples; ++i)
				{
					output[i] += (input[j >> 16] * volume) >> 8;
					j += sourceIndexAdvance;
					volume += volumeChange;
				}
			}
		}

		void mixInSampleAverages(int32* output, const short* input0, const short* input1, int numSamples, int sourceIndexStart, int sourceIndexAdvance, int volume, int volumeChange)
		{
			int j = sourceIndexStart;
			volume /= 2;
			volumeChange /= 2;
			if (volumeChange == 0)
			{
				volume >>= 8;
				for (int i = 0; i < numSamples; ++i)
				{
					const int k = j >> 16;
					output[i] += (input0[k] + input1[k]) * volume;
					j += sourceIndexAdvance;
				}
			}
			else
			{
				if (volume + volumeChange * numSamples < 0)
				{
					numSamples = -volume / volumeChange;
				}
				else if (volume + volumeChange * numSamples > 0x10000)
				{
					numSamples = (0x10000 - volume) / volumeChange;
				}

				for (int i = 0; i < numSamples; ++i)
				{
					const int k = j >> 16;
					output[i] += ((input0[k] + input1[k]) * volume) >> 8;
					j += sourceIndexAdvance;
					volume += volumeChange;
				}
			}
		}
	}



	AudioMixer::AudioMixer(int mixerId) : mRelativeVolume(1.0f), mOutputVolume(1.0f), mMixerId(mixerId), mParent(nullptr) {}

	AudioMixer::~AudioMixer()
	{
		// Remove from hierarchy: Insert all children into own parent
		//  -> Except if this is the root mixer; but when that one is destroyed, all its child will get destroyed afterwards as well
		if (nullptr != mParent)
		{
			for (size_t i = 0; i < mChildren.size(); ++i)
			{
				AudioMixer* child = mChildren[i];
				child->mParent = mParent;
				mParent->mChildren.push_back(child);
			}
		}

		// Stop all playing audio instances
		for (std::map<int, AudioManager::AudioInstance*>::iterator it = mAudioInstances.begin(); it != mAudioInstances.end(); ++it)
		{
			it->second->mPlaybackDone = true;
			it->second->mAudioMixer = nullptr;
		}
	}

	void AudioMixer::addChild(AudioMixer& child)
	{
		if (child.mParent == this)
			return;

		if (nullptr != child.mParent)
		{
			child.mParent->removeChildInternal(child);
		}

		mChildren.push_back(&child);
		child.mParent = this;
	}

	void AudioMixer::clearAudioInstances()
	{
		for (std::map<int, AudioManager::AudioInstance*>::iterator it = mAudioInstances.begin(); it != mAudioInstances.end(); ++it)
		{
			it->second->mPlaybackDone = true;
			it->second->mAudioMixer = nullptr;
		}
		mAudioInstances.clear();
	}

	void AudioMixer::addAudioInstance(AudioManager::AudioInstance& audioInstance)
	{
		mAudioInstances[audioInstance.mID] = &audioInstance;
	}

	void AudioMixer::removeAudioInstance(AudioManager::AudioInstance& audioInstance)
	{
		mAudioInstances.erase(audioInstance.mID);
		audioInstance.mAudioMixer = nullptr;
	}

	void AudioMixer::performAudioMix(const MixerParameters& parameters)
	{
		updateOutputVolume(parameters);
		mixInAllAudioInstances(parameters);
		mixInAllChildren(parameters);
	}

	void AudioMixer::updateOutputVolume(const MixerParameters& parameters)
	{
		mOutputVolume = mRelativeVolume * parameters.mAccumulatedVolume;
	}

	void AudioMixer::mixInAllChildren(const MixerParameters& parameters)
	{
		MixerParameters childParameters = parameters;
		childParameters.mAccumulatedVolume = mOutputVolume;
		for (size_t i = 0; i < mChildren.size(); ++i)
		{
			mChildren[i]->performAudioMix(childParameters);
		}
	}

	void AudioMixer::mixInAllAudioInstances(const MixerParameters& parameters)
	{
		for (std::map<int, AudioManager::AudioInstance*>::iterator it = mAudioInstances.begin(); it != mAudioInstances.end(); ++it)
		{
			mixInAudioInstance(*it->second, parameters.mOutputBuffers, parameters.mOutputSamples, *parameters.mOutputFormat);
		}
	}

	void AudioMixer::mixInAudioInstance(AudioManager::AudioInstance& audioInstance, int32*const* outputBuffer, size_t numOutputSamplesNeeded, const SDL_AudioSpec& outputFormat)
	{
		if (audioInstance.mPaused)
			return;

		AudioBuffer* audioBuffer = audioInstance.mAudioBuffer;
		const int bufferFrequency = audioBuffer->getFrequency();
		const int bufferChannels = audioBuffer->getChannels();

		int32* output[2] = { outputBuffer[0], outputBuffer[1] };

		// Offset playback position?
		if (audioInstance.mPosition < 0)
		{
			const int offset = std::min<int>(numOutputSamplesNeeded, roundToInt((float)(-audioInstance.mPosition) * (float)outputFormat.freq / (float)bufferFrequency));
			if (offset > 0)
			{
				output[0] += offset;
				output[1] += offset;
				numOutputSamplesNeeded -= offset;
				audioInstance.mPosition += roundToInt((float)offset * (float)bufferFrequency / (float)outputFormat.freq);
			}
		}

		if (numOutputSamplesNeeded <= 0)
			return;

		// Parameters
		const float baseVolume = audioInstance.mVolume * mOutputVolume * 256.0f;
		const float baseVolumeChange = audioInstance.mVolumeChange * mOutputVolume * 256.0f;
		const int sourceIndexAdvance = roundToInt(audioInstance.mSpeed * (float)bufferFrequency / (float)outputFormat.freq * 65536.0f);
		int sourceSamplePositionFraction = 0;

		// Mix in samples
		while (numOutputSamplesNeeded > 0)
		{
			// How many samples are available in the current audio buffer?
			short* instanceData[2];
			const int numAvailableInputSamples = audioBuffer->getData(instanceData, audioInstance.mPosition);
			if (numAvailableInputSamples <= 0)
			{
				if (audioInstance.mLoop)
				{
					audioInstance.mPosition = audioInstance.mLoopStart;
					continue;
				}

				if (audioInstance.mStreaming && !audioBuffer->isCompleted())
					break;

				audioInstance.mPlaybackDone = true;
				break;
			}

			// How many samples can we mix in one block?
			int numBlockSamples = numOutputSamplesNeeded;
			if (sourceIndexAdvance > 0)
			{
				const int maxPossibleInputSamples = (numAvailableInputSamples << 16) / sourceIndexAdvance;
				numBlockSamples = std::min(numBlockSamples, maxPossibleInputSamples);
			}

			if (numBlockSamples > 0)
			{
				// Volume
				int volume[2];
				int volumeChange[2];
				if (audioInstance.mUsePan)
				{
					volume[0] = roundToInt(baseVolume * (1.0f - audioInstance.mPanning));
					volume[1] = roundToInt(baseVolume * (1.0f + audioInstance.mPanning));
					volumeChange[0] = roundToInt(baseVolumeChange * (1.0f - audioInstance.mPanning));
					volumeChange[1] = roundToInt(baseVolumeChange * (1.0f + audioInstance.mPanning));
				}
				else
				{
					volume[0] = roundToInt(baseVolume);
					volume[1] = roundToInt(baseVolume);
					volumeChange[0] = roundToInt(baseVolumeChange);
					volumeChange[1] = roundToInt(baseVolumeChange);
				}

				// Mix
				if (outputFormat.channels == 1)
				{
					if (bufferChannels == 1)
						mixInSamples(output[0], instanceData[0], numBlockSamples, sourceSamplePositionFraction, sourceIndexAdvance, volume[0], volumeChange[0]);
					else
						mixInSampleAverages(output[0], instanceData[0], instanceData[1], numBlockSamples, sourceSamplePositionFraction, sourceIndexAdvance, volume[0], volumeChange[0]);
				}
				else
				{
					if (bufferChannels == 1)
					{
						mixInSamples(output[0], instanceData[0], numBlockSamples, sourceSamplePositionFraction, sourceIndexAdvance, volume[0], volumeChange[0]);
						mixInSamples(output[1], instanceData[0], numBlockSamples, sourceSamplePositionFraction, sourceIndexAdvance, volume[1], volumeChange[1]);
					}
					else
					{
						mixInSampleAverages(output[0], instanceData[0], instanceData[1], numBlockSamples, sourceSamplePositionFraction, sourceIndexAdvance, volume[0], volumeChange[0]);
						mixInSampleAverages(output[1], instanceData[0], instanceData[1], numBlockSamples, sourceSamplePositionFraction, sourceIndexAdvance, volume[1], volumeChange[1]);
					}
				}

				output[0] += numBlockSamples;
				output[1] += numBlockSamples;
			}
			else
			{
				numBlockSamples = 1;
			}

			// Advance
			sourceSamplePositionFraction += sourceIndexAdvance * numBlockSamples;
			audioInstance.mPosition += sourceSamplePositionFraction >> 16;
			sourceSamplePositionFraction &= 0xffff;

			if (audioInstance.mTimeout > 0)
			{
				audioInstance.mTimeout -= numBlockSamples;	// Should be related to input samples?
				if (audioInstance.mTimeout <= 0)
				{
					audioInstance.mPlaybackDone = true;
					return;
				}
			}

			// Update instance volume
			if (audioInstance.mVolumeChange != 0.0f)
			{
				const float deltaTime = (float)numBlockSamples / (float)outputFormat.freq;	// In seconds
				audioInstance.mVolume += audioInstance.mVolumeChange * deltaTime;
				if (audioInstance.mVolume < 0.0f)
				{
					audioInstance.mVolume = 0.0f;
					audioInstance.mVolumeChange = 0.0f;
					audioInstance.mPlaybackDone = true;
					return;
				}
				if (audioInstance.mVolume >= 1.0f)
				{
					audioInstance.mVolume = 1.0f;
					audioInstance.mVolumeChange = 0.0f;
				}
			}

			numOutputSamplesNeeded -= numBlockSamples;
		}
	}

	void AudioMixer::removeChildInternal(AudioMixer& child)
	{
		for (size_t i = 0; i < mChildren.size(); ++i)
		{
			if (mChildren[i] == &child)
			{
				mChildren.erase(mChildren.begin() + i);
				child.mParent = nullptr;
				return;
			}
		}
	}

}
