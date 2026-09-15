/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include <rmxbase.h>


template<typename T>
struct VersionRange
{
	T mMinimum;		// Both minimum and maximum value are meant to be inclusive (see "contains()")
	T mMaximum;

	inline VersionRange() : mMinimum(0), mMaximum(0) {}
#if defined(PLATFORM_PS3)
	inline VersionRange(T minimum, T maximum) : mMinimum(minimum), mMaximum(maximum) {}
#else
	inline constexpr VersionRange(T minimum, T maximum) : mMinimum(minimum), mMaximum(maximum) {}
#endif

	inline bool isValid() const  { return (mMinimum <= mMaximum); }

	inline void set(T minimum, T maximum)  { mMinimum = minimum; mMaximum = maximum; }

	inline bool contains(T version) const
	{
		return (version >= mMinimum && version <= mMaximum);
	}

	inline bool intersectsWith(VersionRange<T> other) const
	{
		return (other.mMinimum <= mMaximum && mMinimum <= other.mMaximum);
	}

	inline void serialize(VectorBinarySerializer& serializer)
	{
		serializer.serialize(mMinimum);
		serializer.serialize(mMaximum);
	}

	static inline VersionRange<T> intersectRanges(VersionRange<T> a, VersionRange<T> b)
	{
		VersionRange<T> result;
		result.mMinimum = std::max<T>(a.mMinimum, b.mMinimum);
		result.mMaximum = std::min<T>(a.mMaximum, b.mMaximum);
		return result;
	}
};
