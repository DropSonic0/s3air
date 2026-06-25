#pragma once

#if defined(PLATFORM_PS3)
#include <string>

namespace std {
    class string_view {
    public:
        string_view() : mData(nullptr), mLength(0) {}
        string_view(const char* str) : mData(str), mLength(str ? strlen(str) : 0) {}
        string_view(const char* str, size_t len) : mData(str), mLength(len) {}
        string_view(const std::string& str) : mData(str.data()), mLength(str.length()) {}

        const char* data() const { return mData; }
        size_t length() const { return mLength; }
        size_t size() const { return mLength; }
        bool empty() const { return mLength == 0; }

        char operator[](size_t index) const { return mData[index]; }

        static const size_t npos = -1;

        string_view substr(size_t pos, size_t len = npos) const {
            if (pos > mLength) return string_view();
            if (len == npos || pos + len > mLength) len = mLength - pos;
            return string_view(mData + pos, len);
        }

        size_t find_last_of(char c) const {
            for (int i = (int)mLength - 1; i >= 0; --i) {
                if (mData[i] == c) return i;
            }
            return npos;
        }

    private:
        const char* mData;
        size_t mLength;
    };

    inline bool operator==(string_view lhs, string_view rhs) {
        if (lhs.length() != rhs.length()) return false;
        return memcmp(lhs.data(), rhs.data(), lhs.length()) == 0;
    }
    inline bool operator!=(string_view lhs, string_view rhs) { return !(lhs == rhs); }
}
#endif
