/*
*	rmx Library
*	Copyright (C) 2008-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once


// SinglePtr
template<class CLASS> class SinglePtr
{
private:
#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
	static inline CLASS* mPointer = nullptr;
	static inline int mRefCounter = 0;
#else
	static CLASS* mPointer;
	static int mRefCounter;
#endif
	bool mIsWeak;

public:
	SinglePtr(bool weak = false)
	{
		mIsWeak = weak;
		if (mIsWeak)
			return;

		if (nullptr == mPointer)
		{
			mPointer = new CLASS();
			assert(mRefCounter == 0);
		}
		++mRefCounter;
	}

	~SinglePtr()
	{
		if (mIsWeak)
			return;

		assert(mRefCounter > 0);
		--mRefCounter;
		if (mRefCounter == 0)
			SAFE_DELETE(mPointer);
	}

	bool valid() const			{ return nullptr != mPointer; }

	CLASS& operator*() const	{ return *mPointer; }
	operator CLASS*() const		{ return mPointer; }
	CLASS* operator->() const	{ return mPointer; }
};


// WeakSinglePtr
template<class CLASS> class WeakSinglePtr : public SinglePtr<CLASS>
{
public:
	WeakSinglePtr() : SinglePtr<CLASS>(true) {}
};


#if defined(__CELLOS_LV2__) || defined(__SNC__)
template<class CLASS> CLASS* SinglePtr<CLASS>::mPointer = nullptr;
template<class CLASS> int SinglePtr<CLASS>::mRefCounter = 0;
#endif
