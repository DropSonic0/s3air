#pragma once

#include <cstdio>

// DEBUG: medición de tiempos por ventana de 5 s, imprime por printf (TTY de ProDG, sin diálogos)
struct ProfSlot
{
	const char* mName;
	unsigned int mTotalMs;
	unsigned int mCount;
	unsigned int mLastPrint;
	explicit ProfSlot(const char* name) : mName(name), mTotalMs(0), mCount(0), mLastPrint(0) {}
};

struct ProfScope
{
	ProfSlot& mSlot;
	unsigned int mT0;

	explicit ProfScope(ProfSlot& slot) : mSlot(slot), mT0((unsigned int)SDL_GetTicks()) {}

	~ProfScope()
	{
		const unsigned int now = (unsigned int)SDL_GetTicks();
		mSlot.mTotalMs += now - mT0;
		++mSlot.mCount;
		if (now - mSlot.mLastPrint >= 5000)
		{
			printf("PROF %s: %u calls, %u ms\n", mSlot.mName, mSlot.mCount, mSlot.mTotalMs);
			mSlot.mTotalMs = 0;
			mSlot.mCount = 0;
			mSlot.mLastPrint = now;
		}
	}
};