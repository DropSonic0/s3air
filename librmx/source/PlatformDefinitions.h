/*
*	rmx Library
*	Copyright (C) 2008-2024 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once


// Platforms overview:
//  - PLATFORM_WINDOWS	-> Windows
//  - PLATFORM_LINUX	-> Linux
//  - PLATFORM_MAC		-> macOS
//  - PLATFORM_ANDROID	-> Android
//  - PLATFORM_IOS		-> iOS
//  - PLATFORM_WEB		-> Web version (via emscripten)
//  - PLATFORM_SWITCH	-> Nintendo Switch (homebrew)
//  - PLATFORM_PS3		-> PlayStation 3


// Platform specific
#if defined(__CELLOS_LV2__) || defined(__PS3__) || defined(__SN_TARGET_PS3__)
	#define PLATFORM_PS3
	#define USE_UTF8_PATHS
	#define NO_UNORDERED_CONTAINERS
	#include <stddef.h>
	#include <stdlib.h>
	#include <stdio.h>
	#include <string.h>
	#include <assert.h>
	#include <math.h>

	#ifndef nullptr
		#define nullptr 0
	#endif
	#ifndef override
		#define override
	#endif
	#ifndef final
		#define final
	#endif
	#ifndef noexcept
		#define noexcept
	#endif
	#ifndef constexpr
		#define constexpr const
	#endif
	#ifndef static_assert
		#define static_assert(cond, msg)
	#endif

	#ifndef SDL_LIL_ENDIAN
		#define SDL_LIL_ENDIAN 1234
	#endif
	#ifndef SDL_BIG_ENDIAN
		#define SDL_BIG_ENDIAN 4321
	#endif
	#ifndef SDL_BYTEORDER
		#define SDL_BYTEORDER SDL_BIG_ENDIAN
	#endif

#ifdef __cplusplus
	#include <map>
	#include <set>
	#include <memory>
	#include <string>
	#include <limits>

	// Global namespace fix for missing functions
	using std::floor;
	using std::ceil;

	// Compatibility shims for C++11 features in older compilers
	namespace std {
		template<bool B, typename T, typename F> struct conditional { typedef T type; };
		template<typename T, typename F> struct conditional<false, T, F> { typedef F type; };

		template <typename T, size_t N>
		struct array {
			T elems[N];
			T& operator[](size_t i) { return elems[i]; }
			const T& operator[](size_t i) const { return elems[i]; }
			T* data() { return elems; }
			const T* data() const { return elems; }
			size_t size() const { return N; }
			bool empty() const { return N == 0; }
			T* begin() { return elems; }
			T* end() { return elems + N; }
			const T* begin() const { return elems; }
			const T* end() const { return elems + N; }
		};

		template <typename T>
		struct unique_ptr {
			T* ptr;
			explicit unique_ptr(T* p = 0) : ptr(p) {}
			unique_ptr(const unique_ptr& other) { ptr = const_cast<unique_ptr&>(other).release(); }
			~unique_ptr() { delete ptr; }
			T& operator*() const { return *ptr; }
			T* operator->() const { return ptr; }
			operator bool() const { return ptr != 0; }
			T* get() const { return ptr; }
			T* release() { T* p = ptr; ptr = 0; return p; }
			void reset(T* p = 0) { if (ptr != p) { delete ptr; ptr = p; } }
			unique_ptr& operator=(const unique_ptr& other) { reset(const_cast<unique_ptr&>(other).release()); return *this; }
		};

		template <typename T>
		struct shared_ptr {
			T* ptr;
			int* refCount;
			explicit shared_ptr(T* p = 0) : ptr(p), refCount(new int(1)) {}
			shared_ptr(const shared_ptr& other) : ptr(other.ptr), refCount(other.refCount) { (*refCount)++; }
			template<typename U> shared_ptr(const shared_ptr<U>& other) : ptr(other.ptr), refCount(other.refCount) { (*refCount)++; }
			~shared_ptr() { if (--(*refCount) == 0) { delete ptr; delete refCount; } }
			T& operator*() const { return *ptr; }
			T* operator->() const { return ptr; }
			operator bool() const { return ptr != 0; }
			T* get() const { return ptr; }
			shared_ptr& operator=(const shared_ptr& other) {
				if (this != &other) {
					if (--(*refCount) == 0) { delete ptr; delete refCount; }
					ptr = other.ptr;
					refCount = other.refCount;
					(*refCount)++;
				}
				return *this;
			}
		};

		template <typename T> T& move(T& t) { return t; }
		template <typename T> const T& move(const T& t) { return t; }
		template <typename T> T& forward(T& t) { return t; }

		template<typename K, typename V, typename H = void, typename E = void, typename A = void>
		class unordered_map : public map<K, V> {
		public:
			unordered_map() {}
			template<typename Iter> unordered_map(Iter f, Iter l) : map<K, V>(f, l) {}
		};

		template<typename T, typename H = void, typename E = void, typename A = void>
		class unordered_set : public set<T> {
		public:
			unordered_set() {}
			template<typename Iter> unordered_set(Iter f, Iter l) : set<T>(f, l) {}
		};

		template<typename CHAR>
		class basic_string_view {
		public:
			static const size_t npos = (size_t)-1;

			basic_string_view() : mData(0), mLength(0) {}
			basic_string_view(const CHAR* s) : mData(s), mLength(0) { if (s) while (s[mLength]) ++mLength; }
			basic_string_view(const CHAR* s, size_t l) : mData(s), mLength(l) {}
			basic_string_view(const std::basic_string<CHAR>& s) : mData(s.data()), mLength(s.length()) {}
			const CHAR* data() const { return mData; }
			size_t length() const { return mLength; }
			size_t size() const { return mLength; }
			bool empty() const { return mLength == 0; }
			const CHAR& operator[](size_t i) const { return mData[i]; }
			const CHAR& back() const { return mData[mLength - 1]; }

			bool operator==(const basic_string_view& other) const {
				if (mLength != other.mLength) return false;
				for (size_t i = 0; i < mLength; ++i) if (mData[i] != other.mData[i]) return false;
				return true;
			}
			bool operator!=(const basic_string_view& other) const { return !(*this == other); }
			bool operator==(const CHAR* s) const {
				size_t i = 0;
				for (; i < mLength && s[i]; ++i) if (mData[i] != s[i]) return false;
				return i == mLength && !s[i];
			}
			bool operator!=(const CHAR* s) const { return !(*this == s); }

			int compare(const basic_string_view& other) const {
				size_t len = (mLength < other.mLength) ? mLength : other.mLength;
				for (size_t i = 0; i < len; ++i) {
					if (mData[i] < other.mData[i]) return -1;
					if (mData[i] > other.mData[i]) return 1;
				}
				if (mLength < other.mLength) return -1;
				if (mLength > other.mLength) return 1;
				return 0;
			}

			size_t find(const basic_string_view& s, size_t pos = 0) const {
				if (s.mLength == 0) return pos <= mLength ? pos : npos;
				if (pos + s.mLength > mLength) return npos;
				for (size_t i = pos; i <= mLength - s.mLength; ++i) {
					size_t j = 0;
					for (; j < s.mLength; ++j) {
						if (mData[i + j] != s.mData[j]) break;
					}
					if (j == s.mLength) return i;
				}
				return npos;
			}

			size_t find_last_of(const CHAR* s, size_t pos = npos) const {
				if (mLength == 0 || s == 0) return npos;
				if (pos == npos || pos >= mLength) pos = mLength - 1;
				size_t slen = 0;
				while (s[slen]) ++slen;
				for (size_t i = pos + 1; i > 0; --i) {
					for (size_t j = 0; j < slen; ++j) {
						if (mData[i - 1] == s[j]) return i - 1;
					}
				}
				return npos;
			}

			size_t find_last_of(CHAR c, size_t pos = npos) const {
				if (mLength == 0) return npos;
				if (pos == npos || pos >= mLength) pos = mLength - 1;
				for (size_t i = pos + 1; i > 0; --i) {
					if (mData[i - 1] == c) return i - 1;
				}
				return npos;
			}

			basic_string_view substr(size_t pos, size_t count = npos) const {
				if (pos > mLength) return basic_string_view();
				if (count == npos || pos + count > mLength) count = mLength - pos;
				return basic_string_view(mData + pos, count);
			}

		private:
			const CHAR* mData;
			size_t mLength;
		};

		template<typename CHAR>
		inline bool operator==(const CHAR* s, const basic_string_view<CHAR>& v) { return v == s; }
		template<typename CHAR>
		inline bool operator!=(const CHAR* s, const basic_string_view<CHAR>& v) { return v != s; }
		typedef basic_string_view<char> string_view;
		typedef basic_string_view<wchar_t> wstring_view;

		template <typename T>
		class optional {
		public:
			optional() : mHasValue(false) {}
			optional(const T& value) : mHasValue(true), mValue(value) {}
			bool has_value() const { return mHasValue; }
			operator bool() const { return mHasValue; }
			const T& operator*() const { return mValue; }
			T& operator*() { return mValue; }
			const T* operator->() const { return &mValue; }
			T* operator->() { return &mValue; }
			const T& value() const { return mValue; }
			T& value() { return mValue; }
		private:
			bool mHasValue;
			T mValue;
		};
	}
#endif

#elif defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__)
	#define PLATFORM_WINDOWS
	#if defined(__GNUC__)
		#define USE_UTF8_PATHS
	#endif

#elif __linux__ && !__ANDROID__
	#define PLATFORM_LINUX
	#define USE_UTF8_PATHS		// Linux supports UTF-8 file names instead of wchar_t

#elif __APPLE__
	#include <TargetConditionals.h>
	#if TARGET_IPHONE_SIMULATOR || TARGET_OS_IPHONE
		#define PLATFORM_IOS
	#else
		#define PLATFORM_MAC
	#endif
	#define USE_UTF8_PATHS

#elif __ANDROID__
	#define PLATFORM_ANDROID
	#define USE_UTF8_PATHS

#elif __EMSCRIPTEN__
	#define PLATFORM_WEB
	#define USE_UTF8_PATHS

#elif __SWITCH__
	#define PLATFORM_SWITCH
	#define USE_UTF8_PATHS

#else
	#error "Unsupported platform"
#endif


// Compiler specific
#if defined(_MSC_VER)
	#define FORCE_INLINE __forceinline
	#define RESTRICT __restrict

#elif defined(__GNUC__)
	#define FORCE_INLINE __attribute__((always_inline)) inline
	#define RESTRICT __restrict__

#elif defined(__clang__)
	#define FORCE_INLINE inline
	#define RESTRICT __restrict__

#else
	#define FORCE_INLINE inline
	#define RESTRICT
#endif
