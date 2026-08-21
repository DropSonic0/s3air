/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
#include <chrono>
#else
#include <sys/sys_time.h>
#endif


class HighResolutionTimer
{
public:
	void reset();
	void start();

	inline bool isRunning() const  { return mRunning; }
	double getSecondsSinceStart() const;

protected:
#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
	typedef std::chrono::time_point<std::chrono::high_resolution_clock> TimePoint;
	typedef std::chrono::duration<double> Duration;

	TimePoint mStart;
#else
	typedef uint64 TimePoint;
	typedef double Duration;

	TimePoint mStart = 0;
#endif
	bool mRunning = false;
};


class AccumulativeTimer : protected HighResolutionTimer
{
public:
	void resetTiming();
	void resumeTiming();
	void pauseTiming();

	double getAccumulatedSeconds() const;
	double getAccumulatedSecondsAndRestart();

private:
	Duration mAccumulatedTime;
};
