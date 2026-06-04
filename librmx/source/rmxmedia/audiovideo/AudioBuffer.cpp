/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "rmxmedia.h"


// Static list of loading callbacks
AudioBuffer::LoadCallbackList AudioBuffer::mStaticLoadCallbacks;


AudioBuffer::AudioBuffer() : mPurgedFrames(0), mLength(0), mChannels(2), mFrequency(44100), mPersistent(true), mCompleted(false), mMutexLockCounter(0)
{
}

AudioBuffer::~AudioBuffer()
{
	clearInternal();
}

void AudioBuffer::clear(int frequency, int channels)
{
	RMX_ASSERT(mMutexLockCounter > 0, "Audio buffer mutex should be locked in 'AudioBuffer::clear' method");
	clearInternal();
	mFrequency = clamp(frequency, 22050, 48000);
	mChannels = clamp(channels, 1, 2);
	mCompleted = false;
}

void AudioBuffer::addData(short** data, int length, int frequency, int channels)
{
	RMX_ASSERT(mMutexLockCounter > 0, "Audio buffer mutex should be locked in 'AudioBuffer::addData' method");
	if (nullptr == data || length <= 0)
		return;

	// TODO: Convert data if needed

	int k = 0;
	while (k < length)
	{
		AudioFrame& frame = getWorkingFrame();
		const int remaining = MAX_FRAME_LENGTH - frame.mLength;
		const int len = std::min(remaining, length - k);

		for (int c = 0; c < mChannels; ++c)
		{
			memcpy(&frame.mData[c][frame.mLength], &data[c][k], len * sizeof(short));
		}

		frame.mLength += len;
		mLength += len;
		k += len;
	}
}

void AudioBuffer::addData(float** data, int length, int frequency, int channels)
{
	RMX_ASSERT(mMutexLockCounter > 0, "Audio buffer mutex should be locked in 'AudioBuffer::addData' method");
	if (nullptr == data || length <= 0)
		return;

	// TODO: Convert data if needed

	int k = 0;
	while (k < length)
	{
		AudioFrame& frame = getWorkingFrame();
		const int remaining = MAX_FRAME_LENGTH - frame.mLength;
		const int len = std::min(remaining, length - k);

		for (int c = 0; c < mChannels; ++c)
		{
			short* RESTRICT dst = &frame.mData[c][frame.mLength];
			const float* RESTRICT src = &data[c][k];
			for (int i = 0; i < len; ++i)
			{
				dst[i] = (short)clamp(roundToInt(src[i] * 32767.0f), -32768, 32767);
			}
		}

		frame.mLength += len;
		mLength += len;
		k += len;
	}
}

void AudioBuffer::markPurgeableSamples(int purgePosition)
{
	if (mPersistent)
		return;

	const int purgeFrame = (purgePosition / MAX_FRAME_LENGTH);
	while (mPurgedFrames < purgeFrame && !mFrames.empty())
	{
		AudioFrame* frame = mFrames.front();
		mFrames.erase(mFrames.begin());
		delete[] frame->mBuffer;
		delete frame;
		++mPurgedFrames;
	}
}

bool AudioBuffer::load(const String& source, const String& params)
{
	for (LoadCallbackList::iterator it = mStaticLoadCallbacks.begin(); it != mStaticLoadCallbacks.end(); ++it)
	{
		if ((*it)(this, source, params))
			return true;
	}
	return false;
}

float AudioBuffer::getLengthInSec() const
{
	return (float)mLength / (float)mFrequency;
}

size_t AudioBuffer::getMemoryUsage() const
{
	return mFrames.size() * MAX_FRAME_LENGTH * mChannels * sizeof(short);
}

void AudioBuffer::setPersistent(bool persistent)
{
	mPersistent = persistent;
}

void AudioBuffer::setCompleted(bool completed)
{
	mCompleted = completed;
}

int AudioBuffer::getData(short** output, int position) const
{
	const int frameIndex = position / MAX_FRAME_LENGTH;
	if (frameIndex < mPurgedFrames || frameIndex >= (int)(mPurgedFrames + mFrames.size()))
		return 0;

	const AudioFrame* frame = mFrames[frameIndex - mPurgedFrames];
	const int offset = position % MAX_FRAME_LENGTH;
	if (offset >= frame->mLength)
		return 0;

	output[0] = &frame->mData[0][offset];
	output[1] = &frame->mData[1][offset];
	return frame->mLength - offset;
}

void AudioBuffer::lock()
{
	mMutex.lock();
	++mMutexLockCounter;
}

void AudioBuffer::unlock()
{
	--mMutexLockCounter;
	mMutex.unlock();
}

void AudioBuffer::clearInternal()
{
	for (size_t i = 0; i < mFrames.size(); ++i)
	{
		delete[] mFrames[i]->mBuffer;
		delete mFrames[i];
	}
	mFrames.clear();
	mPurgedFrames = 0;
	mLength = 0;
}

AudioBuffer::AudioFrame& AudioBuffer::getWorkingFrame()
{
	if (mFrames.empty() || mFrames.back()->mLength >= MAX_FRAME_LENGTH)
	{
		AudioFrame* frame = new AudioFrame;
		frame->mBuffer = new short[MAX_FRAME_LENGTH * mChannels];
		frame->mData[0] = &frame->mBuffer[0];
		frame->mData[1] = (mChannels > 1) ? &frame->mBuffer[MAX_FRAME_LENGTH] : &frame->mBuffer[0];
		frame->mLength = 0;
		mFrames.push_back(frame);
	}
	return *mFrames.back();
}
