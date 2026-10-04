/*
*	rmx Library
*	Copyright (C) 2008-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once


namespace rmx
{

	static const constexpr uint32 FNV1a_32_START_VALUE = 0xcbf29ce4u;
	static const constexpr uint32 FNV1a_32_MAGIC_PRIME = 0x1000193u;
	static const constexpr uint64 FNV1a_64_START_VALUE = 0xcbf29ce484222325;
	static const constexpr uint64 FNV1a_64_MAGIC_PRIME = 0x00000100000001b3;

	// Calculate FNV-1a 32-bit hash for data
	uint32 getFNV1a_32(const uint8* data, size_t bytes);
	uint32 startFNV1a_32();
	uint32 addToFNV1a_32(uint32 hash, const uint8* data, size_t bytes);

	// Calculate FNV-1a 64-bit hash for data
	uint64 getFNV1a_64(const uint8* data, size_t bytes);
	uint64 startFNV1a_64();
	uint64 addToFNV1a_64(uint64 hash, const uint8* data, size_t bytes);

	// Compile-time FNV-1a 32-bit and 64-bit of a string
	static constexpr inline uint32 compileTimeFNV_32(const char* string, const uint32 value = FNV1a_32_START_VALUE) noexcept
	{
		return (string[0] == 0) ? value : compileTimeFNV_32(&string[1], static_cast<uint32>((value ^ static_cast<uint32>(string[0])) * FNV1a_32_MAGIC_PRIME));
	}
	static constexpr inline uint64 compileTimeFNV_64(const char* string, const uint64 value = FNV1a_64_START_VALUE) noexcept
	{
		return (string[0] == 0) ? value : compileTimeFNV_64(&string[1], (value ^ static_cast<uint32>(string[0])) * FNV1a_64_MAGIC_PRIME);
	}


	// Calculate Murmur2 64-bit hash for data
	uint64 getMurmur2_64(const uint8* data, size_t bytes);
	uint64 getMurmur2_64(const char* str);
	uint64 getMurmur2_64(const wchar_t* str);
	uint64 getMurmur2_64(const String& str);
	uint64 getMurmur2_64(const WString& str);
	uint64 getMurmur2_64(const std::string& str);
	uint64 getMurmur2_64(const std::wstring& str);
	uint64 getMurmur2_64(std::string_view str);
	uint64 getMurmur2_64(std::wstring_view str);

	namespace detail
	{
		constexpr inline size_t murmur2_len(const char* data, size_t len = 0)
		{
			return data[len] == 0 ? len : murmur2_len(data, len + 1);
		}

		constexpr inline uint64 murmur2_load64(const char* data)
		{
			return ((uint64)(uint8)data[0]) |
				(((uint64)(uint8)data[1]) << 8) |
				(((uint64)(uint8)data[2]) << 16) |
				(((uint64)(uint8)data[3]) << 24) |
				(((uint64)(uint8)data[4]) << 32) |
				(((uint64)(uint8)data[5]) << 40) |
				(((uint64)(uint8)data[6]) << 48) |
				(((uint64)(uint8)data[7]) << 56);
		}

		constexpr inline uint64 murmur2_mix_k(uint64 k)
		{
			return ((k * 0xc6a4a7935bd1e995ull) ^ ((k * 0xc6a4a7935bd1e995ull) >> 47)) * 0xc6a4a7935bd1e995ull;
		}

		constexpr inline uint64 murmur2_chunks(const char* data, size_t chunkCount, uint64 h)
		{
			return chunkCount == 0 ? h : murmur2_chunks(data + 8, chunkCount - 1, (h ^ murmur2_mix_k(murmur2_load64(data))) * 0xc6a4a7935bd1e995ull);
		}

		constexpr inline uint64 murmur2_tail_val(const char* data, size_t rem)
		{
			return (rem >= 7 ? ((uint64)(uint8)data[6]) << 48 : 0) |
				(rem >= 6 ? ((uint64)(uint8)data[5]) << 40 : 0) |
				(rem >= 5 ? ((uint64)(uint8)data[4]) << 32 : 0) |
				(rem >= 4 ? ((uint64)(uint8)data[3]) << 24 : 0) |
				(rem >= 3 ? ((uint64)(uint8)data[2]) << 16 : 0) |
				(rem >= 2 ? ((uint64)(uint8)data[1]) << 8 : 0) |
				(rem >= 1 ? ((uint64)(uint8)data[0]) : 0);
		}

		constexpr inline uint64 murmur2_tail(const char* data, size_t rem, uint64 h)
		{
			return rem == 0 ? h : (h ^ murmur2_tail_val(data, rem)) * 0xc6a4a7935bd1e995ull;
		}

		constexpr inline uint64 murmur2_final_mix(uint64 h)
		{
			return ((h ^ (h >> 47)) * 0xc6a4a7935bd1e995ull) ^ (((h ^ (h >> 47)) * 0xc6a4a7935bd1e995ull) >> 47);
		}

		constexpr inline uint64 constMurmur2_64_impl(const char* data, size_t len)
		{
			return murmur2_final_mix(murmur2_tail(data + (len / 8) * 8, len & 7, murmur2_chunks(data, len / 8, len * 0xc6a4a7935bd1e995ull)));
		}
	}

	// Compile-time constant Murmur2 64-bit hash for a string
#if defined(__cpp_constexpr) && __cpp_constexpr >= 201304L && !defined(__CELLOS_LV2__) && !defined(__SNC__)
	constexpr uint64 constMurmur2_64(const char* data)
	{
		// Code is based on https://github.com/abrandoned/murmur2/blob/master/MurmurHash2.c
		//  -> Namely "MurmurHash64A", i.e. the version optimized for 64-bit architectures
		//  -> We're using a fixed seed of 0

		size_t bytes = 0;
		while (data[bytes] != 0)
			++bytes;

		const uint64 m = 0xc6a4a7935bd1e995ull;
		const int r = 47;
		uint64 h = (uint64)bytes * m;

		const char* end = data + (bytes / 8 * 8);
		while (data != end)
		{
			uint64 k = ((uint64)(uint8)data[0]) + (((uint64)(uint8)data[1]) << 8) + (((uint64)(uint8)data[2]) << 16) + (((uint64)(uint8)data[3]) << 24) + (((uint64)(uint8)data[4]) << 32) + (((uint64)(uint8)data[5]) << 40) + (((uint64)(uint8)data[6]) << 48) + (((uint64)(uint8)data[7]) << 56);
			data += 8;
			k *= m;
			k ^= k >> r;
			k *= m;
			h ^= k;
			h *= m;
		}

		switch (bytes & 0x07)
		{
		case 7:  h ^= ((uint64)(uint8)data[6]) << 48;  RMX_FALLTHROUGH;
		case 6:  h ^= ((uint64)(uint8)data[5]) << 40;  RMX_FALLTHROUGH;
		case 5:  h ^= ((uint64)(uint8)data[4]) << 32;  RMX_FALLTHROUGH;
		case 4:  h ^= ((uint64)(uint8)data[3]) << 24;  RMX_FALLTHROUGH;
		case 3:  h ^= ((uint64)(uint8)data[2]) << 16;  RMX_FALLTHROUGH;
		case 2:  h ^= ((uint64)(uint8)data[1]) << 8;   RMX_FALLTHROUGH;
		case 1:  h ^= ((uint64)(uint8)data[0]);
			h *= m;
		};

		h ^= h >> r;
		h *= m;
		h ^= h >> r;
		return h;
	}
#else
	constexpr inline uint64 constMurmur2_64(const char* data)
	{
		return detail::constMurmur2_64_impl(data, detail::murmur2_len(data));
	}
#endif


	// Calculate CRC32 checksum for data
	uint32 getCRC32(const uint8* data, size_t bytes);

	// Calculate Adler32 checksum for data
	uint32 getAdler32(const uint8* data, size_t bytes);


	// Parse integer, with support for hexidecimal string (starting with "0x") and 64-bit values
	int64 parseInteger(const String& input, size_t& pos);
	int64 parseInteger(const String& input);

	// Create hexadecimal String
	std::string hexString(uint64 value, const char* prefix = "0x");
	std::string hexString(uint64 value, uint32 minDigits, const char* prefix = "0x");

	// Check if an std::string starts with a given other string
	bool startsWith(std::string_view fullString, std::string_view prefix);
	bool startsWith(std::wstring_view fullString, std::wstring_view prefix);
	bool startsWithCaseInsensitive(std::string_view fullString, std::string_view prefix);
	bool startsWithCaseInsensitive(std::wstring_view fullString, std::wstring_view prefix);

	// Check if an std::string ends with a given other string
	bool endsWith(std::string_view fullString, std::string_view suffix);
	bool endsWith(std::wstring_view fullString, std::wstring_view suffix);
	bool endsWithCaseInsensitive(std::string_view fullString, std::string_view suffix);
	bool endsWithCaseInsensitive(std::wstring_view fullString, std::wstring_view suffix);

	// Check if an std::string contains a given other string
	bool containsCaseInsensitive(std::string_view fullString, std::string_view substring);

	// UTF8 conversion
	std::wstring convertFromUTF8(std::string_view str);
	std::string convertToUTF8(std::wstring_view str);


	// Return a string with current date and time, like "2022-06-29_11-42-48"
	std::string getTimestampStringForFilename();

}
