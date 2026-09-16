/*
*	rmx Library
*	Copyright (C) 2008-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "rmxbase.h"


namespace rmx
{
#pragma pack(1)
	struct BmpHeader
	{
		uint8  signature[2];
		uint32 fileSize;
		uint16 creator1;
		uint16 creator2;
		uint32 headerSize;
		uint32 dibHeaderSize;
		int32  width;
		int32  height;
		uint16 numPlanes;
		uint16 bpp;
		uint32 compression;
		uint32 dataSize;
		int32  resolutionX;
		int32  resolutionY;
		uint32 numColors;
		uint32 importantColors;
	};
#pragma pack()

	inline uint32 swapRedBlue(uint32 color)
	{
		return (color & 0xff00ff00) | ((color & 0x00ff0000) >> 16) | ((color & 0x000000ff) << 16);
	}

#define RETURN(errcode) \
	{ \
	outResult.mError = errcode; \
	return (errcode == Bitmap::LoadResult::Error::OK); \
	}



	bool BitmapCodecBMP::canDecode(const String& format) const
	{
		return (format == "bmp");
	}

	bool BitmapCodecBMP::canEncode(const String& format) const
	{
		return (format == "bmp");
	}

	bool BitmapCodecBMP::decode(Bitmap& bitmap, InputStream& stream, Bitmap::LoadResult& outResult)
	{
		MemInputStream mstream(stream);
		return decodeWithStbImage(bitmap, mstream.getCursor(), mstream.getRemaining(), outResult);
	}

	bool BitmapCodecBMP::encode(const Bitmap& bitmap, OutputStream& stream)
	{
		const int width = bitmap.getWidth();
		const int height = bitmap.getHeight();
		const int dataSize = width * height * 4;
		const int headerSize = sizeof(BmpHeader);

		// Header
		BmpHeader header;
		memset(&header, 0, sizeof(BmpHeader));
		memcpy(header.signature, "BM", 2);
		header.fileSize = headerSize + dataSize;
		header.headerSize = headerSize;
		header.dibHeaderSize = 40;
		header.width = width;
		header.height = height;
		header.numPlanes = 1;
		header.bpp = 32;
		header.dataSize = dataSize;
		header.resolutionX = 0xb40;
		header.resolutionY = 0xb40;
		stream << header;

		uint32* output = new uint32[width];

		for (int y = 0; y < height; ++y)
		{
			const uint32* src = bitmap.getPixelPointer(0, height - y - 1);
			for (int x = 0; x < width; ++x)
				output[x] = swapRedBlue(src[x]);
			stream.write(output, width * 4);
		}

		delete[] output;
		return true;
	}

#undef RETURN
}
