/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include <rmxbase.h>

#if !defined(PLATFORM_PS3)
#include <chrono>
#endif


class LagStopwatch
{
public:
#if defined(PLATFORM_PS3)
	inline LagStopwatch(const char* text, int maxMs = 2000) : mText(text), mMaximumMilliseconds(maxMs), mStartTime(0) {}

	inline ~LagStopwatch()
	{
	}
#else
	inline LagStopwatch(const char* text, int maxMs = 2000) : mText(text), mMaximumMilliseconds(maxMs), mStartTime(std::chrono::steady_clock::now()) {}

	inline ~LagStopwatch()
	{
		const uint64 milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - mStartTime).count();
		if (milliseconds > mMaximumMilliseconds)
			RMX_LOG_INFO("LagStopwatch: " << mText << " took " << milliseconds << " ms");
	}
#endif

private:
	const char* mText;
	int mMaximumMilliseconds = 2000;
#if defined(PLATFORM_PS3)
	uint32 mStartTime;
#else
	std::chrono::steady_clock::time_point mStartTime;
#endif
};

#ifdef OXYGEN_SERVER
	#define LAG_STOPWATCH(_text_, _maxMs_) LagStopwatch lagStopwatch_##LINE_NUMBER(_text_, _maxMs_)
#else
	#define LAG_STOPWATCH(_text_, _maxMs_)
#endif
