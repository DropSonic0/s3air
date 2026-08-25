/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include <rmxbase.h>
#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
#include <chrono>

class LagStopwatch
{
public:
	inline LagStopwatch(const char* text, int maxMs = 2000) : mText(text), mMaximumMilliseconds(maxMs), mStartTime(std::chrono::steady_clock::now()) {}

	inline ~LagStopwatch()
	{
		const uint64 milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - mStartTime).count();
		if (milliseconds > mMaximumMilliseconds)
			RMX_LOG_INFO("LagStopwatch: " << mText << " took " << milliseconds << " ms");
	}

private:
	const char* mText;
	int mMaximumMilliseconds = 2000;
	std::chrono::steady_clock::time_point mStartTime;
};
#endif

#if defined(OXYGEN_SERVER) && !defined(__CELLOS_LV2__) && !defined(__SNC__)
	#define LAG_STOPWATCH(_text_, _maxMs_) LagStopwatch lagStopwatch_##LINE_NUMBER(_text_, _maxMs_)
#else
	#define LAG_STOPWATCH(_text_, _maxMs_)
#endif
