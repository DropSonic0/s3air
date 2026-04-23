/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once


namespace rmx
{

	// Fast memory pool that doesn't allow for freeing individual allocations
	class OneTimeAllocPool
	{
	public:
		OneTimeAllocPool();
		~OneTimeAllocPool();

		inline void setPageSize(size_t pageSize)  { mPageSize = pageSize; }

		void clear();
		uint8* allocateMemory(size_t bytes);

	private:
		struct Page
		{
			uint8* mData;
			size_t mSize;
		};
		std::vector<Page> mPages;

		size_t mPageSize;
		uint8* mNextAllocationPointer;
		size_t mRemainingSize;
	};

}
