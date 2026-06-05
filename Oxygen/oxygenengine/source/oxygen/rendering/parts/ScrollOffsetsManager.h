/*
*	Part of the Oxygen Engine / Sonic 3 A.I.R. software distribution.
*	Copyright (C) 2017-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#include "oxygen/rendering/parts/RenderPartsDefinitions.h"


class PlaneManager;

class ScrollOffsetsManager
{
public:
	ScrollOffsetsManager(PlaneManager& planeManager);

	void reset();
	void refresh(const RefreshParameters& refreshParameters);
	void preFrameUpdate();
	void postFrameUpdate();

	inline bool getVerticalScrolling() const				{ return mVerticalScrolling; }
	inline void setVerticalScrolling(bool enable)			{ mVerticalScrolling = enable; }

	inline uint8 getHorizontalScrollMask() const			{ return mHorizontalScrollMask; }
	inline void setHorizontalScrollMask(uint8 scrollMask)	{ mHorizontalScrollMask = scrollMask; }

	inline uint16 getHorizontalScrollTableBase() const				{ return mHorizontalScrollTableBase; }
	inline void setHorizontalScrollTableBase(uint16 vramAddress)	{ mHorizontalScrollTableBase = vramAddress; }

	bool getHorizontalScrollNoRepeat(int setIndex) const;
	void setHorizontalScrollNoRepeat(int setIndex, bool enable);

	void overwriteScrollOffsetH(int setIndex, int index, uint16 value);
	void overwriteScrollOffsetV(int setIndex, int index, uint16 value);

	const uint16* getScrollOffsetsH(int setIndex) const;
	const uint16* getScrollOffsetsV(int setIndex) const;

	inline const Vec2i& getPlaneWScrollOffset() const			 { return mScrollOffsetW; }
	inline void setPlaneWScrollOffset(const Vec2i& scrollOffset) { mScrollOffsetW = scrollOffset; }

	inline int16 getVerticalScrollOffsetBias() const	{ return mVerticalScrollOffsetBias; }
	void setVerticalScrollOffsetBias(int16 bias);

	void serializeSaveState(VectorBinarySerializer& serializer, uint8 formatVersion);

private:
	PlaneManager& mPlaneManager;

	bool mVerticalScrolling;
	uint8 mHorizontalScrollMask;
	uint16 mHorizontalScrollTableBase;

	struct ScrollOffsetSet
	{
		uint16 mScrollOffsetsH[0x100];
		bool mExplicitOverwriteH[0x100];
		uint16 mScrollOffsetsV[0x20];
		bool mExplicitOverwriteV[0x20];
		bool mHorizontalScrollNoRepeat;
	};
	ScrollOffsetSet mSets[4];
	Vec2i mScrollOffsetW;
	int16 mVerticalScrollOffsetBias;

	struct InterpolatedScrollOffsetSet
	{
		bool mValid;
		bool mHasLastScrollOffsets;
		uint16 mInterpolatedScrollOffsetsH[0x100];
		uint16 mInterpolatedScrollOffsetsV[0x20];
		uint16 mLastScrollOffsetsH[0x100];
		uint16 mLastScrollOffsetsV[0x20];
		int16 mDifferenceScrollOffsetsH[0x100];
		int16 mDifferenceScrollOffsetsV[0x20];
	};
	InterpolatedScrollOffsetSet mInterpolatedSets[4];
};
