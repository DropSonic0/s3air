/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#include "rmxbase.h"


namespace rmx
{
#if defined(__arm__) || defined(PLATFORM_PS3)

	// Do not access memory directly, but byte-wise to avoid "SIGBUS illegal alignment" issues (this can happen e.g. in Android Release builds)

	template<>
	uint16 readMemoryUnaligned(const void* pointer)
	{
		const uint8* ptr = (uint8*)pointer;
#if defined(PLATFORM_PS3)
		return ((uint16)ptr[1]) + ((uint16)ptr[0] << 8);
#else
		return ((uint16)ptr[0]) + ((uint16)ptr[1] << 8);
#endif
	}

	template<>
	uint32 readMemoryUnaligned(const void* pointer)
	{
		const uint8* ptr = (uint8*)pointer;
#if defined(PLATFORM_PS3)
		return ((uint32)ptr[3]) + ((uint32)ptr[2] << 8) + ((uint32)ptr[1] << 16) + ((uint32)ptr[0] << 24);
#else
		return ((uint32)ptr[0]) + ((uint32)ptr[1] << 8) + ((uint32)ptr[2] << 16) + ((uint32)ptr[3] << 24);
#endif
	}

	template<>
	uint64 readMemoryUnaligned(const void* pointer)
	{
		const uint8* ptr = (uint8*)pointer;
#if defined(PLATFORM_PS3)
		return ((uint64)ptr[7]) + ((uint64)ptr[6] << 8) + ((uint64)ptr[5] << 16) + ((uint64)ptr[4] << 24) + ((uint64)ptr[3] << 32) + ((uint64)ptr[2] << 40) + ((uint64)ptr[1] << 48) + ((uint64)ptr[0] << 56);
#else
		return ((uint64)ptr[0]) + ((uint64)ptr[1] << 8) + ((uint64)ptr[2] << 16) + ((uint64)ptr[3] << 24) + ((uint64)ptr[4] << 32) + ((uint64)ptr[5] << 40) + ((uint64)ptr[6] << 48) + ((uint64)ptr[7] << 56);
#endif
	}

	template<>
	uint16 readMemoryUnalignedSwapped(const void* pointer)
	{
		const uint8* ptr = (uint8*)pointer;
#if defined(PLATFORM_PS3)
		return ((uint16)ptr[0]) + ((uint16)ptr[1] << 8);
#else
		return ((uint16)ptr[1]) + ((uint16)ptr[0] << 8);
#endif
	}

	template<>
	uint32 readMemoryUnalignedSwapped(const void* pointer)
	{
		const uint8* ptr = (uint8*)pointer;
#if defined(PLATFORM_PS3)
		return ((uint32)ptr[0]) + ((uint32)ptr[1] << 8) + ((uint32)ptr[2] << 16) + ((uint32)ptr[3] << 24);
#else
		return ((uint32)ptr[3]) + ((uint32)ptr[2] << 8) + ((uint32)ptr[1] << 16) + ((uint32)ptr[0] << 24);
#endif
	}

	template<>
	uint64 readMemoryUnalignedSwapped(const void* pointer)
	{
		const uint8* ptr = (uint8*)pointer;
#if defined(PLATFORM_PS3)
		return ((uint64)ptr[0]) + ((uint64)ptr[1] << 8) + ((uint64)ptr[2] << 16) + ((uint64)ptr[3] << 24) + ((uint64)ptr[4] << 32) + ((uint64)ptr[5] << 40) + ((uint64)ptr[6] << 48) + ((uint64)ptr[7] << 56);
#else
		return ((uint64)ptr[7]) + ((uint64)ptr[6] << 8) + ((uint64)ptr[5] << 16) + ((uint64)ptr[4] << 24) + ((uint64)ptr[3] << 32) + ((uint64)ptr[2] << 40) + ((uint64)ptr[1] << 48) + ((uint64)ptr[0] << 56);
#endif
	}

	template<>
	void writeMemoryUnaligned(void* pointer, uint16 value)
	{
		uint8* ptr = (uint8*)pointer;
#if defined(PLATFORM_PS3)
		ptr[1] = (uint8)value;
		ptr[0] = (uint8)(value >> 8);
#else
		ptr[0] = (uint8)value;
		ptr[1] = (uint8)(value >> 8);
#endif
	}

	template<>
	void writeMemoryUnaligned(void* pointer, uint32 value)
	{
		uint8* ptr = (uint8*)pointer;
#if defined(PLATFORM_PS3)
		ptr[3] = (uint8)value;
		ptr[2] = (uint8)(value >> 8);
		ptr[1] = (uint8)(value >> 16);
		ptr[0] = (uint8)(value >> 24);
#else
		ptr[0] = (uint8)value;
		ptr[1] = (uint8)(value >> 8);
		ptr[2] = (uint8)(value >> 16);
		ptr[3] = (uint8)(value >> 24);
#endif
	}

	template<>
	void writeMemoryUnaligned(void* pointer, uint64 value)
	{
		uint8* ptr = (uint8*)pointer;
#if defined(PLATFORM_PS3)
		ptr[7] = (uint8)value;
		ptr[6] = (uint8)(value >> 8);
		ptr[5] = (uint8)(value >> 16);
		ptr[4] = (uint8)(value >> 24);
		ptr[3] = (uint8)(value >> 32);
		ptr[2] = (uint8)(value >> 40);
		ptr[1] = (uint8)(value >> 48);
		ptr[0] = (uint8)(value >> 56);
#else
		ptr[0] = (uint8)value;
		ptr[1] = (uint8)(value >> 8);
		ptr[2] = (uint8)(value >> 16);
		ptr[3] = (uint8)(value >> 24);
		ptr[4] = (uint8)(value >> 32);
		ptr[5] = (uint8)(value >> 40);
		ptr[6] = (uint8)(value >> 48);
		ptr[7] = (uint8)(value >> 56);
#endif
	}

	template<>
	void writeMemoryUnalignedSwapped(void* pointer, uint16 value)
	{
		uint8* ptr = (uint8*)pointer;
#if defined(PLATFORM_PS3)
		ptr[0] = (uint8)value;
		ptr[1] = (uint8)(value >> 8);
#else
		ptr[1] = (uint8)value;
		ptr[0] = (uint8)(value >> 8);
#endif
	}

	template<>
	void writeMemoryUnalignedSwapped(void* pointer, uint32 value)
	{
		uint8* ptr = (uint8*)pointer;
#if defined(PLATFORM_PS3)
		ptr[0] = (uint8)value;
		ptr[1] = (uint8)(value >> 8);
		ptr[2] = (uint8)(value >> 16);
		ptr[3] = (uint8)(value >> 24);
#else
		ptr[3] = (uint8)value;
		ptr[2] = (uint8)(value >> 8);
		ptr[1] = (uint8)(value >> 16);
		ptr[0] = (uint8)(value >> 24);
#endif
	}

	template<>
	void writeMemoryUnalignedSwapped(void* pointer, uint64 value)
	{
		uint8* ptr = (uint8*)pointer;
#if defined(PLATFORM_PS3)
		ptr[0] = (uint8)value;
		ptr[1] = (uint8)(value >> 8);
		ptr[2] = (uint8)(value >> 16);
		ptr[3] = (uint8)(value >> 24);
		ptr[4] = (uint8)(value >> 32);
		ptr[5] = (uint8)(value >> 40);
		ptr[6] = (uint8)(value >> 48);
		ptr[7] = (uint8)(value >> 56);
#else
		ptr[7] = (uint8)value;
		ptr[6] = (uint8)(value >> 8);
		ptr[5] = (uint8)(value >> 16);
		ptr[4] = (uint8)(value >> 24);
		ptr[3] = (uint8)(value >> 32);
		ptr[2] = (uint8)(value >> 40);
		ptr[1] = (uint8)(value >> 48);
		ptr[0] = (uint8)(value >> 56);
#endif
	}

#endif
}
