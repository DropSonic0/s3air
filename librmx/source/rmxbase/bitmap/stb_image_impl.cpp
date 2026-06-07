/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "rmxbase.h"

#if defined(PLATFORM_PS3)

// PS3 PlatformDefinitions.h defines "final" as empty, which conflicts with stb_image's local variables
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
		memcpy(bitmap.getData(), data_stb, width * height * 4);
		stbi_image_free(data_stb);
		outResult.mError = (Bitmap::LoadResult::Error_t)Bitmap::LoadResult::Error::OK;
		return true;
	}
	else
	{
		outResult.mError = (Bitmap::LoadResult::Error_t)Bitmap::LoadResult::Error::INVALID_FILE;
		return false;
	}
}

#endif
