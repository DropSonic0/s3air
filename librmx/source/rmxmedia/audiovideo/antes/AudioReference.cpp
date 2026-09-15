/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "rmxmedia.h"


AudioReference::AudioReference() :
	mInstanceID(0),
	mInstance(nullptr),
	mChangeCounter(-1)
{
}

void AudioReference::setInstanceID(int ID)
{
	mInstanceID = ID;
	mInstance = nullptr;
	mChangeCounter = -1;
}

void AudioReference::updateInstance()
{
	if (mChangeCounter != FTX::Audio->getChangeCounter())
	{
		mInstance = FTX::Audio->findInstance(mInstanceID);
		mChangeCounter = FTX::Audio->getChangeCounter();
	}
}

bool AudioReference::valid()
{
	updateInstance();
	return (nullptr != mInstance);
}

float AudioReference::getPosition()
{
	updateInstance();
	return (nullptr != mInstance) ? ((float)mInstance->mPosition / (float)mInstance->mAudioBuffer->getFrequency()) : 0.0f;
}

float AudioReference::getVolume()
{
	updateInstance();
	return (nullptr != mInstance) ? mInstance->mVolume : 0.0f;
}

float AudioReference::getSpeed()
{
	updateInstance();
	return (nullptr != mInstance) ? mInstance->mSpeed : 1.0f;
}

bool AudioReference::isLooped()
{
	updateInstance();
	return (nullptr != mInstance) ? mInstance->mLoop : false;
}

bool AudioReference::isPaused()
{
	updateInstance();
	return (nullptr != mInstance) ? mInstance->mPaused : false;
}

bool AudioReference::isStreaming()
{
	updateInstance();
	return (nullptr != mInstance) ? mInstance->mStreaming : false;
}

void AudioReference::stop()
{
	updateInstance();
	if (nullptr != mInstance)
	{
		FTX::Audio->removeSound(*this);
	}
}

void AudioReference::setPosition(float position)
{
	updateInstance();
	if (nullptr != mInstance)
	{
		mInstance->mPosition = roundToInt(position * (float)mInstance->mAudioBuffer->getFrequency());
	}
}

void AudioReference::setLoopStartInSamples(int loopStart)
{
	updateInstance();
	if (nullptr != mInstance)
	{
		mInstance->mLoopStart = loopStart;
	}
}

void AudioReference::setVolume(float volume)
{
	updateInstance();
	if (nullptr != mInstance)
	{
		mInstance->mVolume = volume;
		mInstance->mVolumeChange = 0.0f;
	}
}

void AudioReference::setVolumeChange(float volumeChange)
{
	updateInstance();
	if (nullptr != mInstance)
	{
		mInstance->mVolumeChange = volumeChange;
	}
}

void AudioReference::setSpeed(float speed)
{
	updateInstance();
	if (nullptr != mInstance)
	{
		mInstance->mSpeed = speed;
	}
}

void AudioReference::setLoop(bool loop)
{
	updateInstance();
	if (nullptr != mInstance)
	{
		mInstance->mLoop = loop;
	}
}

void AudioReference::setPause(bool pause)
{
	updateInstance();
	if (nullptr != mInstance)
	{
		mInstance->mPaused = pause;
	}
}

void AudioReference::setStreaming(bool strm)
{
	updateInstance();
	if (nullptr != mInstance)
	{
		mInstance->mStreaming = strm;
	}
}

void AudioReference::setTimeout(float timeout)
{
	updateInstance();
	if (nullptr != mInstance)
	{
		mInstance->mTimeout = roundToInt(timeout * (float)mInstance->mAudioBuffer->getFrequency());
	}
}

void AudioReference::setPanning(bool enable, float value)
{
	updateInstance();
	if (nullptr != mInstance)
	{
		mInstance->mUsePan = enable;
		mInstance->mPanning = value;
	}
}
