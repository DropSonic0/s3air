/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*
*	FontSource
*		Source of font data (glyph bitmaps etc.).
*/

#pragma once


class API_EXPORT FontSource
{
public:
	struct GlyphInfo
	{
		uint32 mUnicode;
		Bitmap mBitmap;
		int mAdvance;
		Vec2f mIndent;

		GlyphInfo() : mUnicode(0), mAdvance(0) {}
	};

public:
	FontSource();
	virtual ~FontSource() {}
	virtual const GlyphInfo* getGlyph(uint32 unicode);

	inline int getAscender() const	 { return mAscender; }
	inline int getDescender() const	 { return mDescender; }
	inline int getHeight() const	 { return mAscender + mDescender; }
	inline int getLineHeight() const { return mLineHeight; }

protected:
	virtual bool fillGlyphInfo(GlyphInfo& info)  { return false; }

protected:
	std::map<uint32, GlyphInfo> mGlyphMap;
	int mAscender;		// Maximum height in pixels
	int mDescender;		// Maximum depth in pixels
	int mLineHeight;	// Line height in pixels
};


class API_EXPORT FontSourceStd : public FontSource
{
public:
	virtual ~FontSourceStd() {}
	explicit FontSourceStd(float size);

protected:
	virtual bool fillGlyphInfo(GlyphInfo& info);

private:
	float mSize;
};


class API_EXPORT FontSourceBitmap : public FontSource
{
public:
	virtual ~FontSourceBitmap() {}
	explicit FontSourceBitmap(const String& jsonFilename);

	bool isValid() const  { return mLoadingSucceeded; }

protected:
	virtual bool fillGlyphInfo(GlyphInfo& info);

private:
#if defined(NO_UNORDERED_CONTAINERS)
	std::map<wchar_t, Bitmap>  mCharacterBitmaps;
	std::map<wchar_t, wchar_t> mCharacterRedirects;
#else
	std::unordered_map<wchar_t, Bitmap>  mCharacterBitmaps;
	std::unordered_map<wchar_t, wchar_t> mCharacterRedirects;
#endif
	int mSpaceBetweenCharacters;
	bool mLoadingSucceeded;
};
