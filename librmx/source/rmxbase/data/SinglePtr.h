/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once


// SinglePtr
template<class CLASS> class SinglePtr
{
private:
	#if defined(PLATFORM_PS3)
	static CLASS* mPointer;
	static int mRefCounter;
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

		if (0 == mPointer)
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

	bool valid() const			{ return 0 != mPointer; }

	CLASS& operator*() const	{ return *mPointer; }
	operator CLASS*() const		{ return mPointer; }
	CLASS* operator->() const	{ return mPointer; }
};

#if defined(PLATFORM_PS3)
template<class CLASS> CLASS* SinglePtr<CLASS>::mPointer = 0;
template<class CLASS> int SinglePtr<CLASS>::mRefCounter = 0;
#endif


// WeakSinglePtr
template<class CLASS> class WeakSinglePtr : public SinglePtr<CLASS>
{
public:
	WeakSinglePtr() : SinglePtr<CLASS>(true) {}
};
