/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*
*	AudioMixer
*		Mixes together multiple audio instances.
*/

#pragma once


class AudioManager;

namespace rmx
{

	class API_EXPORT AudioMixer
	{
	friend class AudioManager;

	public:
		struct MixerParameters
		{
			int32* mOutputBuffers[2];	// One for each channel
			size_t mOutputSamples;
			const SDL_AudioSpec* mOutputFormat;
			float mAccumulatedVolume;

			MixerParameters() : mOutputSamples(0), mOutputFormat(nullptr), mAccumulatedVolume(1.0f) { mOutputBuffers[0] = nullptr; mOutputBuffers[1] = nullptr; }
		};

	public:
		AudioMixer(int mixerId);
		virtual ~AudioMixer();

		void addChild(AudioMixer& child);

		void clearAudioInstances();
		void addAudioInstance(AudioManager::AudioInstance& audioInstance);
		void removeAudioInstance(AudioManager::AudioInstance& audioInstance);

		inline float getVolume() const		  { return mRelativeVolume; }
		void setVolume(float relativeVolume)  { mRelativeVolume = relativeVolume; }

	public:
		virtual void performAudioMix(const MixerParameters& parameters);

	protected:
		void updateOutputVolume(const MixerParameters& parameters);
		void mixInAllChildren(const MixerParameters& parameters);
		void mixInAllAudioInstances(const MixerParameters& parameters);
		void mixInAudioInstance(AudioManager::AudioInstance& audioInstance, int32*const* outputBuffer, size_t numOutputSamplesNeeded, const SDL_AudioSpec& outputFormat);

	protected:
		float mRelativeVolume;
		float mOutputVolume;
		std::map<int, AudioManager::AudioInstance*> mAudioInstances;

	private:
		bool mixAudioBufferInner(AudioManager::AudioInstance& audioInstance, int32** output, size_t numOutputSamplesNeeded, const SDL_AudioSpec& outputFormat, int sourceIndexAdvance);
		void removeChildInternal(AudioMixer& child);

	private:
		int mMixerId;
		AudioMixer* mParent;
		std::vector<AudioMixer*> mChildren;
	};

}
