/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*
*	SpriteAtlas
*		Dynamic sprite/texture atlas.
*/

#pragma once

class API_EXPORT SpriteAtlasBase
{
public:
	struct Page
	{
		int mIndex;
		Vec2i mPageSize;
		Page() : mIndex(0) {}
	};
	struct Sprite
	{
		uint32 mKey;
		Page mPage;
		Recti mRect;
		Sprite() : mKey(0) {}
	};

public:
	SpriteAtlasBase();
	virtual ~SpriteAtlasBase();

	void clear();
	bool add(uint32 key, const Vec2i& size);
	void rebuild();

	bool valid(uint32 key);
	bool getSprite(uint32 key, Sprite& sprite);

	inline int getNumPages() const  { return (int)mPages.size(); }
	bool getPage(int index, Page& page);

protected:
	struct SpriteInfo;

	bool internalAdd(uint32 key, const Vec2i& size);
	SpriteInfo* getSpriteInfo(uint32 key);

private:
	static bool compareSpriteInfoBySize(const SpriteInfo& first, const SpriteInfo& second);

protected:
	Vec2i mPageSize;		// That size is a bit small for usual text rendering, but okay for pixelized rendering as used by Oxygen
	int mPadding;

	struct Node
	{
		Node* mChildNode[2];
		Recti mRect;
		bool mUsed;

		inline Node() : mUsed(false) { mChildNode[0] = 0; mChildNode[1] = 0; }
		inline Node(const Recti& rct) : mRect(rct), mUsed(false) { mChildNode[0] = 0; mChildNode[1] = 0; }
		inline ~Node()  { clear(); }

		void clear();
		Node* insert(const Vec2i& size, int padding);
	};

	struct PageInfo
	{
		Node mRootNode;
	};
	std::vector<PageInfo> mPages;

	struct SpriteInfo
	{
		uint32 mKey;
		int mPageIndex;
		Recti mRect;
		SpriteInfo() : mKey(0xffffffff), mPageIndex(-1) {}
	};
	std::map<uint32, SpriteInfo> mSprites;
};



#ifdef RMX_WITH_OPENGL_SUPPORT

class API_EXPORT SpriteAtlas : protected SpriteAtlasBase
{
public:
	struct Sprite
	{
		Texture* mTexture;
		Vec2f mUVStart;
		Vec2f mUVEnd;
		Sprite() : mTexture(0) {}
	};

public:
	SpriteAtlas();
	virtual ~SpriteAtlas();

	void clear();
	int add(const Bitmap& bmp);
	int add(const Bitmap& bmp, const Recti& rect);
	void rebuild();

	bool valid(int handle);
	bool getSprite(int handle, Sprite& sprite);

	int getPageCount()  { return (int)mPages.size(); }
	const Texture* getPage(int num);

private:
	int internalAdd(const Bitmap& bmp, const Recti* rect, bool updateTexture = true);

private:
	struct PageData
	{
		Bitmap mBitmap;
		Texture mTexture;
	};
	std::vector<PageData> mPageData;
};

#endif
