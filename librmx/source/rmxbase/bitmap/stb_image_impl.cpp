/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "rmxbase.h"

// PlatformDefinitions.h on some platforms may define "final" as empty, which conflicts with stb_image's local variables
#ifdef final
#undef final
#endif

#define STB_IMAGE_IMPLEMENTATION
#define STBI_NO_STDIO
#define STBI_ASSERT(x)
#define STBI_NO_SIMD
#include "stb_image.h"

bool rmx::decodeWithStbImage(Bitmap& bitmap, const void* data, size_t size, Bitmap::LoadResult& outResult)
{
	int width, height, n;
	unsigned char* data_stb = stbi_load_from_memory((const unsigned char*)data, (int)size, &width, &height, &n, 4);
	if (data_stb)
	{
		bitmap.create(width, height);
		uint32* dst = bitmap.getData();
		const uint8* src = (const uint8*)data_stb;
		const int pixels = width * height;
		for (int i = 0; i < pixels; ++i)
		{
			const uint8 r = src[i * 4 + 0];
			const uint8 g = src[i * 4 + 1];
			const uint8 b = src[i * 4 + 2];
			const uint8 a = src[i * 4 + 3];
#if defined(PLATFORM_PS3) || defined(RMX_PLATFORM_PS3) || defined(__CELLOS_LV2__) || defined(__SNC__)
			// ARGB format: A (bits 24..31), R (bits 16..23), G (bits 8..15), B (bits 0..7)
			dst[i] = ((uint32)a << 24) | ((uint32)r << 16) | ((uint32)g << 8) | (uint32)b;
#else
			dst[i] = ((const uint32*)src)[i];
#endif
		}
		stbi_image_free(data_stb);
		outResult.mError = Bitmap::LoadResult::Error::OK;
		return true;
	}
	else
	{
		outResult.mError = Bitmap::LoadResult::Error::INVALID_FILE;
		return false;
	}
}
