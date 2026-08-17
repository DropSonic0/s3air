/*
*	rmx Library
*	Copyright (C) 2008-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*
*	Texture
*		OpenGL texture (2D or cubemap) wrapper class.
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
#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
	Texture(const Texture& other) = delete;
	Texture& operator=(const Texture& other) = delete;
#else
	Texture(const Texture& other) : mHandle(other.mHandle), mType(other.mType), mFormat(other.mFormat), mWidth(other.mWidth), mHeight(other.mHeight), mFilterLinear(other.mFilterLinear), mHasMipmaps(other.mHasMipmaps) {}
	Texture& operator=(const Texture& other) {
		if (this != &other) {
			mHandle = other.mHandle;
			mType = other.mType;
			mFormat = other.mFormat;
			mWidth = other.mWidth;
			mHeight = other.mHeight;
			mFilterLinear = other.mFilterLinear;
			mHasMipmaps = other.mHasMipmaps;
		}
		return *this;
	}
#endif
	Texture(Texture&& other);
	~Texture();

	void generate();	// Just calls "glGenTextures", nothing else

	void create(GLenum type = GL_TEXTURE_2D);
	void create(GLint format = rmx::OpenGLHelper::FORMAT_RGBA);
	void create(int width, int height, GLint format = rmx::OpenGLHelper::FORMAT_RGBA);
	void create(const Vec2i& size, GLint format = rmx::OpenGLHelper::FORMAT_RGBA);
	void createCubemap(GLint format = rmx::OpenGLHelper::FORMAT_RGBA);
	void createCubemap(int width, int height, GLint format = rmx::OpenGLHelper::FORMAT_RGBA);
	void createCubemap(const Vec2i& size, GLint format = rmx::OpenGLHelper::FORMAT_RGBA);

	void load(const void* data, int width, int height);
	void load(const void* data, const Vec2i& size);
	void load(const Bitmap& bitmap);
	void load(const String& filename);
	void loadCubemap(const String& filename);

	void updateAll(const void* data);
	void updateRect(const void* data, const Recti& rect);
	void updateRect(const Bitmap& bitmap, int px, int py);

	void copyFramebuffer(const Recti& rect);
	void copyFramebufferCubemap(const Recti& rect, int side);

	void buildMipmaps();

	inline GLuint getHandle() const	{ return mHandle; }
	inline GLenum getType() const	{ return mType; }
	inline GLint getFormat() const	{ return mFormat; }
	inline int getWidth() const		{ return mWidth; }
	inline int getHeight() const	{ return mHeight; }
	inline Vec2i getSize() const	{ return Vec2i(mWidth, mHeight); }
	inline Recti getRect() const	{ return Recti(0, 0, mWidth, mHeight); }
	inline float getAspectRatio() const  { return (float)mWidth / (float)mHeight; }

	void bind() const;
	void unbind() const;

	void setFilterNearest();
	void setFilterLinear();
	void setWrapClamp();
	void setWrapRepeat();
	void setWrapRepeatMirror();

	inline GLuint operator*() const  { return mHandle; }

	static GLenum getDefaultDataFormat(GLint internalFormat);

private:
	void initialize();
	bool checkHandle() const;

private:
	mutable GLuint mHandle = 0;
	GLenum mType = 0;
	GLint  mFormat = 0;
	int    mWidth = 0;
	int    mHeight = 0;
	bool   mFilterLinear = true;
	bool   mHasMipmaps = false;
};

#endif
