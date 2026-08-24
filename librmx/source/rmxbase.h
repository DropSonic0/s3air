/*
*	rmx Library
*	Copyright (C) 2008-2026 by Eukaryot
*
*	Published under the GNU GPLv3 open source software license, see license.txt
*	or https://www.gnu.org/licenses/gpl-3.0.en.html
*/

#pragma once

// Version number
#define RMXBASE_VERSION 0x00040100

// General includes
#include <cmath>
#include <float.h>
#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
#include <memory.h>
#else
#include <memory>
#endif
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <iostream>
#include <assert.h>
#include <vector>
#include <stack>
#include <list>
#include <set>
#include <map>
#include <algorithm>

#if !defined(__CELLOS_LV2__) && !defined(__SNC__)
#include <unordered_set>
#include <unordered_map>
#else
#include <limits>
#include <climits>

#ifndef UINT32_MAX
#define UINT32_MAX 0xffffffffu
#endif

namespace std {
    inline std::string to_string(int val) { char buf[32]; sprintf(buf, "%d", val); return buf; }
    inline std::string to_string(unsigned int val) { char buf[32]; sprintf(buf, "%u", val); return buf; }
    inline std::string to_string(long val) { char buf[32]; sprintf(buf, "%ld", val); return buf; }
    inline std::string to_string(unsigned long val) { char buf[32]; sprintf(buf, "%lu", val); return buf; }
    inline std::string to_string(long long val) { char buf[64]; sprintf(buf, "%lld", val); return buf; }
    inline std::string to_string(unsigned long long val) { char buf[64]; sprintf(buf, "%llu", val); return buf; }
    inline std::string to_string(float val) { char buf[64]; sprintf(buf, "%f", val); return buf; }
    inline std::string to_string(double val) { char buf[64]; sprintf(buf, "%f", val); return buf; }

    template<typename T>
    class shared_ptr {
    public:
        template<typename U> friend class shared_ptr;

        shared_ptr() : mPtr(nullptr), mRefCount(nullptr) {}
        explicit shared_ptr(T* p) : mPtr(p), mRefCount(p ? new int(1) : nullptr) {}
        
        shared_ptr(const shared_ptr& other) : mPtr(other.mPtr), mRefCount(other.mRefCount) {
            if (mRefCount) {
                ++(*mRefCount);
            }
        }

        template<typename U>
        shared_ptr(const shared_ptr<U>& other) : mPtr(other.mPtr), mRefCount(other.mRefCount) {
            if (mRefCount) {
                ++(*mRefCount);
            }
        }
        
        ~shared_ptr() {
            release();
        }
        
        shared_ptr& operator=(const shared_ptr& other) {
            if (this != &other) {
                release();
                mPtr = other.mPtr;
                mRefCount = other.mRefCount;
                if (mRefCount) {
                    ++(*mRefCount);
                }
            }
            return *this;
        }
        
        T* get() const { return mPtr; }
        T& operator*() const { return *mPtr; }
        T* operator->() const { return mPtr; }
        operator bool() const { return mPtr != nullptr; }
        
    private:
        void release() {
            if (mRefCount) {
                --(*mRefCount);
                if (*mRefCount == 0) {
                    delete mPtr;
                    delete mRefCount;
                }
            }
        }
        
        T* mPtr;
        int* mRefCount;
    };

    template<typename K, typename V, typename H=void, typename E=void, typename A=void>
    class unordered_map : public std::map<K, V> {
    };

    template<typename T, typename H=void, typename E=void, typename A=void>
    class unordered_set : public std::set<T> {
    };


    struct error_code {
        int value() const { return 0; }
        operator bool() const { return false; }
        void clear() {}
    };
}

#if defined(__CELLOS_LV2__) || defined(__SNC__)
using std::sqrt;
using std::floor;
using std::ceil;
using std::abs;
using std::exp;
using std::expf;
using std::cos;
using std::sin;
using std::pow;
using std::log10;
using std::log10f;
#endif
#endif

// Libraries
#include "rmxbase/_jsoncpp/json/json.h"	// Uses its own namespace "Json"

// RMX modules
#include "PlatformDefinitions.h"
#include "export.h"
#include "rmxbase/base/Types.h"
#include "rmxbase/base/Basics.h"
#include "rmxbase/base/StdHelpers.h"
#include "rmxbase/base/ErrorHandler.h"
#include "rmxbase/tools/BitArray.h"
#include "rmxbase/tools/BitFlagSet.h"
#include "rmxbase/math/Math.h"
#include "rmxbase/data/CArray.h"
#include "rmxbase/memory/ObjectPool.h"
#include "rmxbase/memory/OneTimeAllocPool.h"
#include "rmxbase/memory/UnalignedMemory.h"
#include "rmxbase/data/SingleInstance.h"
#include "rmxbase/data/Singleton.h"
#include "rmxbase/data/SinglePtr.h"
#include "rmxbase/data/SmartPtr.h"
#include "rmxbase/data/GlobalObjectPtr.h"
#include "rmxbase/data/WeakPtr.h"
#include "rmxbase/memory/RC4Encryption.h"
#include "rmxbase/memory/String.h"
#include "rmxbase/memory/UTF8Conversion.h"
#include "rmxbase/tools/Tools.h"
#include "rmxbase/file/FileHandle.h"
#include "rmxbase/file/FileIO.h"
#include "rmxbase/file/FileProvider.h"
#include "rmxbase/file/RealFileProvider.h"
#include "rmxbase/file/FileSystem.h"
#include "rmxbase/file/FileCrawler.h"
#include "rmxbase/file/JsonHelper.h"
#include "rmxbase/memory/InputStream.h"
#include "rmxbase/memory/OutputStream.h"
#include "rmxbase/memory/BinarySerializer.h"
#include "rmxbase/memory/VectorBinarySerializer.h"
#include "rmxbase/memory/RmxDeflate.h"
#include "rmxbase/memory/ZlibDeflate.h"
#include "rmxbase/bitmap/Color.h"
#include "rmxbase/bitmap/BitmapCodecs.h"
#include "rmxbase/bitmap/PaletteBitmap.h"
#include "rmxbase/bitmap/BitmapView.h"
#include "rmxbase/tools/Logging.h"


// Library linking via pragma
#if defined(PLATFORM_WINDOWS) && defined(RMX_LIB)
	#pragma comment(lib, "rmxbase.lib")
#endif



// Global object pointers
namespace FTX
{
	extern GlobalObjectPtr<rmx::FileSystem> FileSystem;
}


// This include depends on FTX::FileSystem, so add it afterwards
#include "rmxbase/memory/StringImpl.h"


// Initialization
namespace rmxbase
{
	void initialize();
	void getBuildInfo(String& info);
}

#define INIT_RMX  { rmxbase::initialize(); }
