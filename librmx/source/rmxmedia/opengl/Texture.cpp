/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "rmxmedia.h"

#ifdef RMX_WITH_OPENGL_SUPPORT

#if defined(PLATFORM_PS3)
static void swizzleRGBAtoARGB(uint32_t* data, int count)
{
	for (int i = 0; i < count; ++i)
	{
		uint32_t rgba = data[i];
		// Input (RGBA): 0xRRGGBBAA
		// Output (ARGB): 0xAARRGGBB
		data[i] = (rgba >> 8) | (rgba << 24);
	}
}
#endif

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
	mType = GL_TEXTURE_2D;
	mFormat = rmx::OpenGLHelper::FORMAT_RGBA;
	mWidth = 0;
	mHeight = 0;
	mFilterLinear = true;
	mHasMipmaps = false;
}

void Texture::generate() const
{
	if (mHandle == 0)
	{
		glGenTextures(1, &mHandle);
	}
}

void Texture::create(int type)
{
	generate();
	if (type != 0)
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

	bind();
#if defined(PLATFORM_PS3)
	glTexImage2D(mType, 0, mFormat, mWidth, mHeight, 0, GL_ARGB_SCE, GL_UNSIGNED_BYTE, nullptr);
#else
	glTexImage2D(mType, 0, mFormat, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
#endif
	setFilterLinear();
	setWrapClamp();
}

void Texture::createCubemap(int format)
{
	create(GL_TEXTURE_CUBE_MAP);
	mFormat = format;
}

void Texture::createCubemap(int width, int height, int format)
{
	createCubemap(format);
	mWidth = width;
	mHeight = height;

	bind();
	for (int i = 0; i < 6; ++i)
	{
#if defined(PLATFORM_PS3)
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mFormat, mWidth, mHeight, 0, GL_ARGB_SCE, GL_UNSIGNED_BYTE, nullptr);
#else
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, mFormat, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
#endif
	}
	setFilterLinear();
	setWrapClamp();
}

void Texture::load(const void* data, int width, int height)
{
	mWidth = width;
	mHeight = height;
	generate();
	bind();

#if defined(PLATFORM_PS3)
	if (data != nullptr)
	{
		// We need to swizzle the data for PS3's ARGB format
		std::vector<uint32_t> swizzledData((uint32_t*)data, (uint32_t*)data + (width * height));
		swizzleRGBAtoARGB(&swizzledData[0], width * height);
		glTexImage2D(mType, 0, mFormat, mWidth, mHeight, 0, GL_ARGB_SCE, GL_UNSIGNED_BYTE, &swizzledData[0]);
	}
	else
	{
		glTexImage2D(mType, 0, mFormat, mWidth, mHeight, 0, GL_ARGB_SCE, GL_UNSIGNED_BYTE, nullptr);
	}
#else
	glTexImage2D(mType, 0, mFormat, mWidth, mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
#endif

	setFilterLinear();
	setWrapClamp();
}

void Texture::load(const Bitmap& bitmap)
{
	if (bitmap.empty())
		return;
	load(bitmap.getData(), bitmap.getWidth(), bitmap.getHeight());
}

void Texture::load(const String& filename)
{
	Bitmap bmp(filename);
	load(bmp);
}

void Texture::loadCubemap(const String& filename)
{
	// Not implemented for now as it requires loading 6 faces
}

void Texture::updateRect(const void* data, const Recti& rect)
{
	if (mHandle == 0)
		return;

	bind();
#if defined(PLATFORM_PS3)
	if (data != nullptr)
	{
		std::vector<uint32_t> swizzledData((uint32_t*)data, (uint32_t*)data + (rect.width * rect.height));
		swizzleRGBAtoARGB(&swizzledData[0], rect.width * rect.height);
		glTexSubImage2D(mType, 0, rect.x, rect.y, rect.width, rect.height, GL_ARGB_SCE, GL_UNSIGNED_BYTE, &swizzledData[0]);
	}
#else
	glTexSubImage2D(mType, 0, rect.x, rect.y, rect.width, rect.height, GL_RGBA, GL_UNSIGNED_BYTE, data);
#endif
}

void Texture::updateRect(const Bitmap& bitmap, int px, int py)
{
	if (bitmap.empty())
		return;
	updateRect(bitmap.getData(), Recti(px, py, bitmap.getWidth(), bitmap.getHeight()));
}

void Texture::copyFramebuffer(const Recti& rect)
{
	if (mHandle == 0)
		return;
	bind();
	glCopyTexSubImage2D(mType, 0, 0, 0, rect.x, rect.y, rect.width, rect.height);
}

void Texture::copyFramebufferCubemap(const Recti& rect, int side)
{
	if (mHandle == 0)
		return;
	bind();
	glCopyTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + side, 0, 0, 0, rect.x, rect.y, rect.width, rect.height);
}

void Texture::buildMipmaps()
{
	if (mHandle == 0)
		return;
	bind();
	glGenerateMipmap(mType);
	mHasMipmaps = true;
}

void Texture::bind() const
{
	generate();
	glBindTexture(mType, mHandle);
}

void Texture::unbind() const
{
	glBindTexture(mType, 0);
}

void Texture::setFilterNearest()
{
	bind();
	glTexParameteri(mType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(mType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	mFilterLinear = false;
}

void Texture::setFilterLinear()
{
	bind();
	glTexParameteri(mType, GL_TEXTURE_MIN_FILTER, mHasMipmaps ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
	glTexParameteri(mType, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	mFilterLinear = true;
}

void Texture::setWrapClamp()
{
	bind();
	glTexParameteri(mType, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(mType, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void Texture::setWrapRepeat()
{
	bind();
	glTexParameteri(mType, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(mType, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

void Texture::setWrapRepeatMirror()
{
	bind();
#if defined(GL_MIRRORED_REPEAT)
	glTexParameteri(mType, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTexParameteri(mType, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
#endif
}

int Texture::getDefaultDataFormat(int internalFormat)
{
#if defined(PLATFORM_PS3)
	return GL_ARGB_SCE;
#else
	return GL_RGBA;
#endif
}

bool Texture::checkHandle() const
{
	return mHandle != 0;
}

#endif
