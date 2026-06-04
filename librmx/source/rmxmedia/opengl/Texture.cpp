/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "rmxmedia.h"

#ifdef RMX_WITH_OPENGL_SUPPORT

Texture::Texture() : mHandle(0), mType(0), mFormat(0), mWidth(0), mHeight(0), mFilterLinear(true), mHasMipmaps(false)
{
	initialize();
}

Texture::Texture(const Bitmap& bitmap) : mHandle(0), mType(0), mFormat(0), mWidth(0), mHeight(0), mFilterLinear(true), mHasMipmaps(false)
{
	initialize();
	load(bitmap);
}

Texture::Texture(const String& filename) : mHandle(0), mType(0), mFormat(0), mWidth(0), mHeight(0), mFilterLinear(true), mHasMipmaps(false)
{
	initialize();
	load(filename);
}

Texture::~Texture()
{
	if (mHandle != 0)
	{
		glDeleteTextures(1, &mHandle);
	}
}

void Texture::initialize()
{
	mType = 0;
	mFormat = 0;
	mWidth = 0;
	mHeight = 0;
	mFilterLinear = true;
	mHasMipmaps = false;
}

void Texture::generate()
{
	if (mHandle == 0)
	{
		glGenTextures(1, &mHandle);
	}
}

void Texture::create(int type)
{
	generate();
	mType = type;
}

void Texture::create_format(int format)
{
	create();
	mFormat = format;
}

void Texture::create(int width, int height, int format)
{
	create();
	mWidth = width;
	mHeight = height;
	mFormat = format;
}

void Texture::createCubemap(int format)
{
	create(0); // GL_TEXTURE_CUBE_MAP
	mFormat = format;
}

void Texture::createCubemap(int width, int height, int format)
{
	createCubemap(format);
	mWidth = width;
	mHeight = height;
}

void Texture::load(const void* data, int width, int height)
{
	// Dummy implementation
}

void Texture::load(const Bitmap& bitmap)
{
	// Dummy implementation
}

void Texture::load(const String& filename)
{
	// Dummy implementation
}

void Texture::loadCubemap(const String& filename)
{
	// Dummy implementation
}

void Texture::updateRect(const void* data, const Recti& rect)
{
}

void Texture::updateRect(const Bitmap& bitmap, int px, int py)
{
}

void Texture::copyFramebuffer(const Recti& rect)
{
}

void Texture::copyFramebufferCubemap(const Recti& rect, int side)
{
}

void Texture::buildMipmaps()
{
}

void Texture::bind() const
{
}

void Texture::unbind() const
{
}

void Texture::setFilterNearest()
{
}

void Texture::setFilterLinear()
{
}

void Texture::setWrapClamp()
{
}

void Texture::setWrapRepeat()
{
}

void Texture::setWrapRepeatMirror()
{
}

int Texture::getDefaultDataFormat(int internalFormat)
{
	return 0;
}

bool Texture::checkHandle() const
{
	return true;
}

#endif
