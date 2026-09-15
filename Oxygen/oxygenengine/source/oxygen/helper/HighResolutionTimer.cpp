/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "oxygen/pch.h"
#include "oxygen/helper/HighResolutionTimer.h"


#if defined(__CELLOS_LV2__) || defined(__SNC__)
static HighResolutionTimer::TimePoint getNowPS3()
{
	return (HighResolutionTimer::TimePoint)sys_time_get_system_time();
}
#endif


void HighResolutionTimer::reset()
{
	mRunning = false;
}

void HighResolutionTimer::start()
{
#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
	mStart = std::chrono::high_resolution_clock::now();
#else
	mStart = getNowPS3();
#endif
	mRunning = true;
}

double HighResolutionTimer::getSecondsSinceStart() const
{
	if (mRunning)
	{
#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
		const Duration duration = (std::chrono::high_resolution_clock::now() - mStart);
		return duration.count();
#else
		return (double)(getNowPS3() - mStart) / 1000000.0;
#endif
	}
	return 0.0;
}


void AccumulativeTimer::resetTiming()
{
	reset();
#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
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
#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
		mAccumulatedTime += (std::chrono::high_resolution_clock::now() - mStart);
#else
		mAccumulatedTime += (double)(getNowPS3() - mStart) / 1000000.0;
#endif
		mRunning = false;
	}
}

double AccumulativeTimer::getAccumulatedSeconds() const
{
	Duration totalDuration = mAccumulatedTime;
	if (mRunning)
	{
#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
		totalDuration += (std::chrono::high_resolution_clock::now() - mStart);
#else
		totalDuration += (double)(getNowPS3() - mStart) / 1000000.0;
#endif
	}
#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
	return totalDuration.count();
#else
	return totalDuration;
#endif
}

double AccumulativeTimer::getAccumulatedSecondsAndRestart()
{
#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
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
	const TimePoint now = getNowPS3();
	Duration totalDuration = mAccumulatedTime;
	if (mRunning)
	{
		totalDuration += (double)(now - mStart) / 1000000.0;
	}

	mStart = now;
	mAccumulatedTime = 0.0;
	mRunning = true;
	return totalDuration;
#endif
}
