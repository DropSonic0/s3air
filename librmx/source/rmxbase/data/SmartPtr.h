/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once


// Base class for reference counted objects
class RefCounted
{
public:
	RefCounted() : mRefCounter(0) {}
	void refInc()  { ++mRefCounter; }
	bool refDec()  { --mRefCounter; return (mRefCounter > 0); }

private:
	int mRefCounter;
};


// Smart pointer for RefCounted classes
template<class CLASS> class SmartPtr
{
public:
	SmartPtr() : mPtr(0) {}
	SmartPtr(CLASS* ptr) : mPtr(0) { set(ptr); }
	SmartPtr(const SmartPtr<CLASS>& ptr) : mPtr(0) { set(*ptr); }
	~SmartPtr()  { clear(); }

	void clear()
	{
		if (0 == mPtr)
			return;
		if (!mPtr->refDec())
			delete mPtr;
		mPtr = 0;
	}

	void set(CLASS* ptr)
	{
		if (ptr == mPtr)
			return;
		if (mPtr)
			clear();
		if (ptr)
		{
			ptr->refInc();
			mPtr = ptr;
		}
	}

	void operator=(CLASS* ptr) { set(ptr); }
	CLASS* operator*() const   { return mPtr; }
	operator CLASS*() const    { return mPtr; }
	CLASS& operator->() const  { return *mPtr; }

private:
	CLASS* mPtr;
};
