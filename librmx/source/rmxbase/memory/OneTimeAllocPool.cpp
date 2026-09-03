/*
*	rmx Library
*	Copyright (C) 2008-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "rmxbase.h"


namespace rmx
{
	OneTimeAllocPool::~OneTimeAllocPool()
	{
		clear();
	}

	void OneTimeAllocPool::clear()
	{
		for (Page& page : mPages)
		{
#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
			delete[] page.mData;
#else
			if (page.mData) free(page.mData);
#endif
		}
		mPages.clear();
		mNextAllocationPointer = nullptr;
		mRemainingSize = 0;
	}

	uint8* OneTimeAllocPool::allocateMemory(size_t bytes)
	{
		// Always round up to a multiple of 16 bytes on PS3 for Altivec alignment, or 8 bytes on other 64-bit machines
	#if defined(__CELLOS_LV2__) || defined(__SNC__)
		bytes = ((bytes + 15) & ~(size_t)0x0f);
	#elif !defined(PLATFORM_VITA)
		bytes = ((bytes + 7) & ~(size_t)0x07);
	#else
		// Let's use 4 bytes for the PSVITA
		bytes = ((bytes + 3) & ~(size_t)0x03);
	#endif
		if (bytes > mRemainingSize)
		{
			RMX_CHECK(bytes <= mPageSize, "Too large memory allocation of " << bytes << " bytes", return nullptr);

			// Add a new page
			Page& page = vectorAdd(mPages);
#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
			page.mData = new uint8[mPageSize];
#else
			page.mData = static_cast<uint8*>(memalign(128, mPageSize));
#endif
			page.mSize = mPageSize;

			mNextAllocationPointer = page.mData;
			mRemainingSize = mPageSize;
		}

		uint8* ptr = mNextAllocationPointer;
		mNextAllocationPointer += bytes;
		mRemainingSize -= bytes;
		return ptr;
	}
}
