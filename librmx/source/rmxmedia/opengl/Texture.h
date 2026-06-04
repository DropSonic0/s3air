/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

#ifdef RMX_WITH_OPENGL_SUPPORT

#include "OpenGLHelper.h"

class Texture
{
public:
	Texture();
	Texture(const Bitmap& bitmap);
	Texture(const String& filename);
	~Texture();

	void generate();	// Just calls "glGenTextures", nothing else

	void create(int type = 0);
	void create_format(int format);
	void create(int width, int height, int format = rmx::OpenGLHelper::FORMAT_RGBA);
	void createCubemap(int format = rmx::OpenGLHelper::FORMAT_RGBA);
	void createCubemap(int width, int height, int format = rmx::OpenGLHelper::FORMAT_RGBA);

	void load(const void* data, int width, int height);
	void load(const Bitmap& bitmap);
	void load(const String& filename);
	void loadCubemap(const String& filename);

	void updateRect(const void* data, const Recti& rect);
	void updateRect(const Bitmap& bitmap, int px, int py);

	void copyFramebuffer(const Recti& rect);
	void copyFramebufferCubemap(const Recti& rect, int side);

	void buildMipmaps();

	inline unsigned int getHandle() const	{ return mHandle; }
	inline int getType() const	{ return mType; }
	inline int getFormat() const	{ return mFormat; }
	inline int getWidth() const		{ return mWidth; }
	inline int getHeight() const	{ return mHeight; }
	inline Recti getRect() const	{ return Recti(0, 0, mWidth, mHeight); }
	inline float getAspectRatio() const  { return (float)mWidth / (float)mHeight; }

	void bind() const;
	void unbind() const;

	void setFilterNearest();
	void setFilterLinear();
	void setWrapClamp();
	void setWrapRepeat();
	void setWrapRepeatMirror();

	inline unsigned int operator*() const  { return mHandle; }

	static int getDefaultDataFormat(int internalFormat);

private:
	void initialize();
	bool checkHandle() const;

private:
	mutable unsigned int mHandle;
	int mType;
	int mFormat;
	int mWidth;
	int mHeight;
	bool mFilterLinear;
	bool mHasMipmaps;
};

#endif
