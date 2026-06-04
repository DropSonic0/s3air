/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*
*	Font
*		Rendering of text.
*/

#pragma once

#include "Font.h"


struct FontProcessingData
{
	Bitmap mBitmap;
	int mBorderLeft;
	int mBorderRight;
	int mBorderTop;
	int mBorderBottom;

	FontProcessingData() : mBorderLeft(0), mBorderRight(0), mBorderTop(0), mBorderBottom(0) {}
};


class FontProcessor
{
public:
	virtual ~FontProcessor() {}
	virtual void process(FontProcessingData& data) = 0;
};


class ShadowFontProcessor : public FontProcessor
{
public:
	virtual ~ShadowFontProcessor() {}
	inline explicit ShadowFontProcessor(Vec2i shadowOffset, float shadowBlur, Color shadowColor) :
		mShadowOffset(shadowOffset),
		mShadowBlur(shadowBlur),
		mShadowColor(shadowColor)
	{}

	inline explicit ShadowFontProcessor(Vec2i shadowOffset, float shadowBlur, float shadowAlpha) :
		mShadowOffset(shadowOffset),
		mShadowBlur(shadowBlur),
		mShadowColor(Color(0.0f, 0.0f, 0.0f, shadowAlpha))
	{}

	virtual void process(FontProcessingData& data) override;

private:
	Vec2i mShadowOffset;
	float mShadowBlur;
	Color mShadowColor;
};


class OutlineFontProcessor : public FontProcessor
{
public:
	virtual ~OutlineFontProcessor() {}
	inline explicit OutlineFontProcessor(Color outlineColor, int range, bool rectangularOutline) :
		mOutlineColor(outlineColor),
		mRange(range),
		mRectangularOutline(rectangularOutline)
	{}

	virtual void process(FontProcessingData& data) override;

private:
	Color mOutlineColor;
	int mRange;
	bool mRectangularOutline;
};


class GradientFontProcessor : public FontProcessor
{
public:
	virtual ~GradientFontProcessor() {}
	inline explicit GradientFontProcessor()
	{}

	virtual void process(FontProcessingData& data) override;
};
