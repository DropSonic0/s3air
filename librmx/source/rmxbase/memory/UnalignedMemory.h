/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once


#include "rmxbase/base/Types.h"


namespace rmx
{
	template<typename T>
	T readMemoryUnaligned(const void* pointer) { return *(T*)pointer; }

	template<typename T>
	T readMemoryUnalignedSwapped(const void* pointer) { return swapBytes<T>(*(T*)pointer); }

	template<typename T>
	T readMemoryUnalignedLE(const void* pointer)
	{
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
		return readMemoryUnalignedSwapped<T>(pointer);
#else
		return readMemoryUnaligned<T>(pointer);
#endif
	}

	template<typename T>
	void writeMemoryUnaligned(void* pointer, T value) { *(T*)pointer = value; }

	template<typename T>
	void writeMemoryUnalignedSwapped(void* pointer, T value) { *(T*)pointer = swapBytes<T>(value); }

#if defined(__arm__) || defined(PLATFORM_PS3)
	template<> uint16 readMemoryUnaligned(const void* pointer);
	template<> uint32 readMemoryUnaligned(const void* pointer);
	template<> uint64 readMemoryUnaligned(const void* pointer);

	template<> uint16 readMemoryUnalignedSwapped(const void* pointer);
	template<> uint32 readMemoryUnalignedSwapped(const void* pointer);
	template<> uint64 readMemoryUnalignedSwapped(const void* pointer);

	template<> void writeMemoryUnaligned(void* pointer, uint16 value);
	template<> void writeMemoryUnaligned(void* pointer, uint32 value);
	template<> void writeMemoryUnaligned(void* pointer, uint64 value);

	template<> void writeMemoryUnalignedSwapped(void* pointer, uint16 value);
	template<> void writeMemoryUnalignedSwapped(void* pointer, uint32 value);
	template<> void writeMemoryUnalignedSwapped(void* pointer, uint64 value);
#endif
}
