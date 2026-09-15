/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*
*	Audio
*		Music and sounds output.
*/

#pragma once


class AudioReference;

namespace rmx
{
	class AudioMixer;


	// Audio manager
	class API_EXPORT AudioManager
#if defined(PLATFORM_PS3)
		: public ThreadBase
#endif
	{
	public:
		struct AudioInstance
		{
			int mID;							// Unique audio instance ID, invalid if 0
			AudioBuffer* mAudioBuffer;	// The audio buffer used as a source, must not be a nullptr
			AudioMixer* mAudioMixer;		// Audio mixer this is played in
			int mPosition;						// Position in the audio buffer, in samples
			int mTimeout;						// Time until playback gets stopped in samples, or 0 if not used
			int mLoopStart;						// If looping is enabled, jump back to this sample position
			float mVolume;					// Volume in range [0.0f, 1.0f]
			float mVolumeChange;				// Volume change rate per second, usually 0.0f
			float mSpeed;					// Playback speed, usually 1.0f
			float mPanning;					// Left/right panning value in range [-1.0f, +1.0f], usually 0.0f
			bool mLoop;						// Set if sound playback should be looped
			bool mPaused;					// Set when sound playback is paused
			bool mUsePan;					// Set if panning should be used
			bool mStreaming;				// Set if reaching the end of the audio buffer should not stop the playback, just temporily pause it until more data comes in
			bool mPlaybackDone;				// Gets set by audio mixer when playback should stop now

			AudioInstance() : mID(0), mAudioBuffer(nullptr), mAudioMixer(nullptr), mPosition(0), mTimeout(0), mLoopStart(0), mVolume(1.0f), mVolumeChange(0.0f), mSpeed(1.0f), mPanning(0.0f), mLoop(false), mPaused(false), mUsePan(false), mStreaming(false), mPlaybackDone(false) {}
		};

		struct PlaybackOptions
		{
			AudioBuffer* mAudioBuffer;
			float mVolume;
			float mVolumeChange;
			int mAudioMixerId;
			float mSpeed;
			float mPosition;
			bool mLoop;
			bool mStreaming;

			PlaybackOptions() : mAudioBuffer(nullptr), mVolume(1.0f), mVolumeChange(0.0f), mAudioMixerId(0), mSpeed(1.0f), mPosition(0.0f), mLoop(false), mStreaming(false) {}
		};

	public:
		AudioManager();
		~AudioManager();

		void initialize(int sample_freq = 44100, int channels = 2, int audioBufferSamples = 1024);
		void exit();

		void clear();

		void playAudio(bool onoff);
		bool getAudioState();

		void lockAudio();
		void unlockAudio();

		void regularUpdate(float timeElapsed);	// Should best be called once every frame

		void setGlobalVolume(float volume);

		template<typename T>
		T& createAudioMixer(int mixerId, int parentMixerId = 0)
		{
			RMX_ASSERT(mixerId != 0, "Root audio mixer (with ID 0) can't be replaced");
			T* audioMixer = new T(mixerId);
			registerAudioMixer(*audioMixer, parentMixerId);
			return *audioMixer;
		}

		AudioMixer* getAudioMixerByID(int mixerId) const;
		void deleteAudioMixerByID(int mixerId);

		float getAudioMixerVolumeByID(int mixerId) const;
		void setAudioMixerVolumeByID(int mixerId, float relativeVolume);

		bool addSound(const PlaybackOptions& playbackOptions, AudioReference& ref);
		int addSound(AudioBuffer* audiobuffer, bool streaming = false);							// Deprecated
		bool addSound(AudioBuffer* audiobuffer, AudioReference& ref, bool streaming = false);	// Deprecated

		void removeSound(AudioReference& ref);
		void removeAllSounds();

		AudioInstance* findInstance(int ID);

		inline int getChangeCounter() const  { return mChangeCounter; }

		inline int getOutputBufferSize() const		  { return mFormat.samples; }
		inline int getOutputFrequency() const		  { return mFormat.freq; }
		inline uint32 getGlobalPlayedSamples() const  { return mPlayedSamples; }
		inline double getGlobalPlaybackTime() const   { return (double)mPlayedSamples / (double)mFormat.freq; }

	private:
		void registerAudioMixer(AudioMixer& audioMixer, int parentMixerId);

		void removeInstance(int ID);
		void processRemoveIDs();

		static void mixAudioStatic(void* _userdata, uint8* outputStream, int outputBytes);
		void mixAudio(uint8* outputStream, int outputBytes);

#if defined(PLATFORM_PS3)
	protected:
		virtual void threadFunc() override;
#endif

	private:
		SDL_AudioDeviceID mAudioDeviceID;		// Audio device opened by SDL
		SDL_AudioSpec mFormat;						// Audio format
		uint32 mAudioLocks;						// Set if audio device is locked right now (needed to allow for nested audio locking)
#if defined(PLATFORM_PS3)
		SDL_mutex* mMutex;
#endif
		std::map<int, AudioInstance> mInstances;	// Map of all active audio instances by their ID
		std::vector<int> mRemoveIDs;				// Audio instance IDs that got invalid during audio mixing
		int mNextFreeID;						// ID to use for next audio instance created
		int mChangeCounter;						// Changed whenever an audio instance gets created or invalidated
		uint32 mPlayedSamples;					// Number of samples played (this takes about one day to overflow at 48 kHz)

		float mTimeSinceLastUpdate;

#if defined(PLATFORM_PS3)
		uint32 mAudioPort;
		bool mAudioInitialized;
		bool mAudioStarted;
#endif

		// Mixers
		std::map<int, AudioMixer*> mAudioMixers;
		AudioMixer& mRootMixer;				// Root audio mixer
	};


	// WAV loader
	class API_EXPORT WavLoader
	{
	public:
		static bool load(AudioBuffer* buffer, const String& source, const String& params);
	};

}
