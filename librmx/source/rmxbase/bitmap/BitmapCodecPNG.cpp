/*
*	rmx Library
*	Copyright (C) 2008-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "rmxbase.h"

// For data decompression, either use zlib (which is faster) or alternatively the RmxDeflate class
#define USE_ZLIB


namespace rmx
{
	namespace
	{
		uint32 readUint32LE(const uint8* pointer)
		{
			// Read as little endian
			return ((uint32)pointer[0]) + ((uint32)pointer[1] << 8) + ((uint32)pointer[2] << 16) + ((uint32)pointer[3] << 24);
		}

		uint32 readUint32BE(const uint8* pointer)
		{
			// Read as big endian
			return ((uint32)pointer[0] << 24) + ((uint32)pointer[1] << 16) + ((uint32)pointer[2] << 8) + ((uint32)pointer[3]);
		}
	}


#define PNG_IHDR 0x49484452
#define PNG_IDAT 0x49444154
#define PNG_IEND 0x49454e44
#define PNG_PLTE 0x504c5445

	const uint8 PNGSignature[8] = { 0x89, 0x50, 0x4e, 0x47, 0x0d, 0x0a, 0x1a, 0x0a };

	// PNG header
	struct PNGHeader
	{
		uint32 width;
		uint32 height;
		uint8  bitdepth;
		uint8  colortype;
		uint8  compression;
		uint8  filter;
		uint8  interlace;
	};

#define RETURN(errcode) \
	{ \
	outResult.mError = errcode; \
	return (errcode == Bitmap::LoadResult::Error::OK); \
	}



	bool BitmapCodecPNG::canDecode(const String& format) const
	{
		return (format == "png");
	}

	bool BitmapCodecPNG::canEncode(const String& format) const
	{
		return (format == "png");
	}

	bool BitmapCodecPNG::decode(Bitmap& bitmap, InputStream& stream, Bitmap::LoadResult& outResult)
	{
		MemInputStream mstream(stream);
		return decodeWithStbImage(bitmap, mstream.getCursor(), mstream.getRemaining(), outResult);
	}

	bool BitmapCodecPNG::encode(const Bitmap& bitmap, OutputStream& stream)
	{
		// Save image data to memory in PNG format
		if (bitmap.empty())
			return false;

		int width = bitmap.getWidth();
		int height = bitmap.getHeight();

		// Setup PNG header
		PNGHeader header;
		header.width = swapBytes32(bitmap.getWidth());
		header.height = swapBytes32(bitmap.getHeight());
		header.bitdepth = 8;
		header.colortype = 6;
		header.compression = 0;
		header.filter = 0;
		header.interlace = 0;

		// Pack image data
		uint8* tmpdata = new uint8[(width * 4 + 1)*height];
		for (int line = 0; line < height; ++line)
		{
			tmpdata[line*(width * 4 + 1)] = 0;
			memcpy(&tmpdata[line*(width * 4 + 1) + 1], bitmap.getPixelPointer(0, line), width * 4);
		}
		int outsize = 0;
		uint8* output = Deflate::encode(outsize, tmpdata, (width * 4 + 1)*height);			// TODO: Optionally use ZlibDeflate here as well
		const uint32 adler = swapBytes32(rmx::getAdler32(tmpdata, (width * 4 + 1)*height));
		delete[] tmpdata;

		// Write PNG data
		const int bufsize = 3 * 12 + 13 + outsize + 6;
		uint8* buffer = new uint8[bufsize];
		uint8* mem = buffer;

		// Write chunks
		for (int chunknum = 0; chunknum < 3; ++chunknum)
		{
			uint8* chunkStart = mem;
			mem += 8;
			uint32 type = 0;
			uint32 length = 0;
			if (chunknum == 0)
			{
				type = PNG_IHDR;
				length = 13;
				memcpy(mem, &header, length);
			}
			else if (chunknum == 1)
			{
				type = PNG_IDAT;
				length = outsize + 6;
				mem[0] = 0x78;		// zlib header
				mem[1] = 0xda;		// zlib header
				memcpy(&mem[2], output, outsize);
				*(uint32*)&chunkStart[length + 4] = adler;
			}
			else
			{
				type = PNG_IEND;
			}

			*(uint32*)&chunkStart[0] = swapBytes32(length);
			*(uint32*)&chunkStart[4] = swapBytes32(type);
			const uint32 crc = rmx::getCRC32(chunkStart + 4, length + 4);
			*(uint32*)&chunkStart[length + 8] = swapBytes32(crc);
			mem += length + 4;
		}
		delete[] output;

		stream.write(PNGSignature, 8);
		stream.write(buffer, bufsize);
		delete[] buffer;

		return true;
	}
}
