/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "oxygen/pch.h"
#include "oxygen/helper/HighResolutionTimer.h"


void HighResolutionTimer::reset()
{
	mRunning = false;
}

void HighResolutionTimer::start()
{
#if !defined(PLATFORM_PS3)
	mStart = std::chrono::high_resolution_clock::now();
#else
	mStart = SDL_GetTicks();
#endif
	mRunning = true;
}

double HighResolutionTimer::getSecondsSinceStart() const
{
	if (mRunning)
	{
#if !defined(PLATFORM_PS3)
		const Duration duration = (std::chrono::high_resolution_clock::now() - mStart);
		return duration.count();
#else
		return (double)(SDL_GetTicks() - mStart) / 1000.0;
#endif
	}
	return 0.0;
}


void AccumulativeTimer::resetTiming()
{
	reset();
#if !defined(PLATFORM_PS3)
	mAccumulatedTime = Duration::zero();
#else
	mAccumulatedTime = 0.0;
#endif
}

void AccumulativeTimer::resumeTiming()
{
	if (!mRunning)
	{
		start();
	}
}

void AccumulativeTimer::pauseTiming()
{
	if (mRunning)
	{
#if !defined(PLATFORM_PS3)
		mAccumulatedTime += (std::chrono::high_resolution_clock::now() - mStart);
#else
		mAccumulatedTime += (double)(SDL_GetTicks() - mStart) / 1000.0;
#endif
		mRunning = false;
	}
}

double AccumulativeTimer::getAccumulatedSeconds() const
{
	Duration totalDuration = mAccumulatedTime;
	if (mRunning)
	{
#if !defined(PLATFORM_PS3)
		totalDuration += (std::chrono::high_resolution_clock::now() - mStart);
#else
		totalDuration += (double)(SDL_GetTicks() - mStart) / 1000.0;
#endif
	}
#if !defined(PLATFORM_PS3)
	return totalDuration.count();
#else
	return totalDuration;
#endif
}

double AccumulativeTimer::getAccumulatedSecondsAndRestart()
{
#if !defined(PLATFORM_PS3)
	const TimePoint now = std::chrono::high_resolution_clock::now();
	Duration totalDuration = mAccumulatedTime;
	if (mRunning)
	{
		totalDuration += (now - mStart);
	}

	mStart = now;
	mAccumulatedTime = Duration::zero();
	mRunning = true;
	return totalDuration.count();
#else
	const TimePoint now = SDL_GetTicks();
	Duration totalDuration = mAccumulatedTime;
	if (mRunning)
	{
		totalDuration += (double)(now - mStart) / 1000.0;
	}

	mStart = now;
	mAccumulatedTime = 0.0;
	mRunning = true;
	return totalDuration;
#endif
}
