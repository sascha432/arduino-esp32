/*
 WString.h - String library for Wiring & Arduino
 ...mostly rewritten by Paul Stoffregen...
 Copyright (c) 2009-10 Hernando Barragan.  All right reserved.
 Copyright 2011, Paul Stoffregen, paul@pjrc.com

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef String_class_h
#define String_class_h
#ifdef __cplusplus

#include <pgmspace.h>

#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cctype>

#include <utility>
#include <type_traits>

#ifdef _MSC_VER
#include <misc_wstring.h>
#else
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <pgmspace.h>
#include "KFCBaseLibrary/include/misc_wstring.h"
#endif
// #include <stdlib.h>
// #include <string.h>
// #include <ctype.h>
// #include <pgmspace.h>
// #include <stdint.h>

// An inherited class for holding the result of a concatenation.  These
// result objects are assumed to be writable by subsequent concatenations.
class StringSumHelper;

 // an abstract class used as a means to proide a unique pointer type
 // but really has no body
class __FlashStringHelper;
#ifndef FPSTR
#define FPSTR(pstr_pointer) (reinterpret_cast<const __FlashStringHelper *>(pstr_pointer))
#endif
#ifndef F
#define F(string_literal) (FPSTR(PSTR(string_literal)))
#endif

// support libraries that expect this name to be available
// replace with `using StringSumHelper = String;` in case something wants this constructible
class StringSumHelper;

// The string class
class String {
        // use a function pointer to allow for "if (s)" without the
        // complications of an operator bool(). for more information, see:
        // http://www.artima.com/cppsource/safebool.html
        typedef void (String::*StringIfHelperType)() const;
        void StringIfHelper() const {
        }

    public:
        // constructors
        // creates a copy of the initial value.
        // if the initial value is null or invalid, or if memory allocation
        // fails, the string will be marked as invalid (i.e. "if (s)" will
        // be false).
        String() __attribute__((always_inline)) { // See init()
            init();
        }
        String(const char *cstr);
        String(const String &str);
        String(const __FlashStringHelper *str);
#ifdef __GXX_EXPERIMENTAL_CXX0X__
        String(String &&rval);
        String(StringSumHelper &&rval);
#endif
        explicit String(char c);
        explicit String(unsigned char, unsigned char base = 10);
        explicit String(int, unsigned char base = 10);
        explicit String(unsigned int, unsigned char base = 10);
        explicit String(long, unsigned char base = 10);
        explicit String(unsigned long, unsigned char base = 10);
        explicit String(long long /* base 10 */);
        explicit String(unsigned long long /* base 10 */);
        explicit String(float, unsigned char decimalPlaces = 2);
        explicit String(double, unsigned char decimalPlaces = 2);
        ~String() {
            invalidate();
        }

        // memory management
        // return true on success, false on failure (in which case, the string
        // is left unchanged).  reserve(0), if successful, will validate an
        // invalid string (i.e., "if (s)" will be true afterwards)
        bool reserve(unsigned int size);
        unsigned int length(void) const {
            return buffer() ? len() : 0;
        }
        void clear(void) {
            setLen(0);
        }
        bool isEmpty(void) const {
            return length() == 0;
        }

        // creates a copy of the assigned value.  if the value is null or
        // invalid, or if the memory allocation fails, the string will be
        // marked as invalid ("if (s)" will be false).
        String &operator =(const String &rhs);
        String &operator =(const char *cstr);
        String &operator =(const __FlashStringHelper *str);
#ifdef __GXX_EXPERIMENTAL_CXX0X__
        String & operator =(String &&rval);
        String & operator =(StringSumHelper &&rval);
#endif

        // concatenate (works w/ built-in types)

        // returns true on success, false on failure (in which case, the string
        // is left unchanged).  if the argument is null or invalid, the
        // concatenation is considered unsuccessful.
        unsigned char concat(const String &str);
        unsigned char concat(const char *cstr);
        unsigned char concat(char c);
        unsigned char concat(unsigned char c);
        unsigned char concat(int num);
        unsigned char concat(unsigned int num);
        unsigned char concat(long num);
        unsigned char concat(unsigned long num);
        unsigned char concat(float num);
        unsigned char concat(double num);
        unsigned char concat(const __FlashStringHelper * str);

        // if there's not enough memory for the concatenated value, the string
        // will be left unchanged (but this isn't signalled in any way)
        // if there's not enough memory for the concatenated value, the string
        // will be left unchanged (but this isn't signalled in any way)
        template <typename T>
        String &operator +=(const T &rhs) {
            concat(rhs);
            return *this;
        }

        friend StringSumHelper & operator +(const StringSumHelper &lhs, const String &rhs);
        friend StringSumHelper & operator +(const StringSumHelper &lhs, const char *cstr);
        friend StringSumHelper & operator +(const StringSumHelper &lhs, char c);
        friend StringSumHelper & operator +(const StringSumHelper &lhs, unsigned char num);
        friend StringSumHelper & operator +(const StringSumHelper &lhs, int num);
        friend StringSumHelper & operator +(const StringSumHelper &lhs, unsigned int num);
        friend StringSumHelper & operator +(const StringSumHelper &lhs, long num);
        friend StringSumHelper & operator +(const StringSumHelper &lhs, unsigned long num);
        friend StringSumHelper & operator +(const StringSumHelper &lhs, float num);
        friend StringSumHelper & operator +(const StringSumHelper &lhs, double num);
        friend StringSumHelper & operator +(const StringSumHelper &lhs, const __FlashStringHelper *rhs);

        // comparison (only works w/ Strings and "strings")
        operator StringIfHelperType() const {
            return buffer() ? &String::StringIfHelper : 0;
        }
        int compareTo(const String &s) const;
        bool operator ==(const String &rhs) const {
            return equals(rhs);
        }
        bool operator ==(const char *cstr) const {
            return equals(cstr);
        }
        bool operator !=(const String &rhs) const {
            return !equals(rhs);
        }
        bool operator !=(const char *cstr) const {
            return !equals(cstr);
        }
        bool operator <(const String &rhs) const;
        bool operator >(const String &rhs) const;
        bool operator <=(const String &rhs) const;
        bool operator >=(const String &rhs) const;
        bool equalsConstantTime(const String &s) const;

    // additional operators for char and const __FashStringHelper *
    public:
        bool operator ==(char ch) const {
            return equals(ch);
        }
        bool operator ==(const __FlashStringHelper *fstr) const {
            return strcmp_P(buffer(), reinterpret_cast<PGM_P>(fstr)) == 0;
        }
        bool operator !=(char ch) const {
            return !equals(ch);
        }
        bool operator !=(const __FlashStringHelper *fstr) const {
            return strcmp_P(buffer(), reinterpret_cast<PGM_P>(fstr)) != 0;
        }

    public:

        // character access
        char charAt(unsigned int index) const {
            return operator [](index);
        }
        void setCharAt(unsigned int index, char c);
        char operator [](unsigned int index) const;
        char &operator [](unsigned int index);
        void getBytes(unsigned char *buf, unsigned int bufsize, unsigned int index = 0) const;
        void toCharArray(char *buf, unsigned int bufsize, unsigned int index = 0) const {
            getBytes((unsigned char *) buf, bufsize, index);
        }
        const char *c_str() const { return buffer(); }
        char *begin() { return wbuffer(); }
        char *end() { return wbuffer() + length(); }
        const char *begin() const { return c_str(); }
        const char *end() const { return c_str() + length(); }

        String substring(unsigned int beginIndex) const {
            return substring(beginIndex, len());
        }
        String substring(unsigned int beginIndex, unsigned int endIndex) const;

        // Pass the biggest integer if the count is not specified.
        // The remove method below will take care of truncating it at the end of the string.
        void remove(unsigned int index, unsigned int count = (unsigned int)-1);

        // added return type
        String &toLowerCase(void);
        String &toUpperCase(void);

        // parsing/conversion
        long toInt(void) const;
        float toFloat(void) const;
        double toDouble(void) const;

    // additional equals methods with ignorecase and support for char, const char *, const __FlashStringHelper * and String
    public:
        //
        // equals()
        //
        // changed behaviour:
        // any nullptr argument will return false -> String.equals(nullptr) does not return true if String is empty

        // the terminating NUL byte is not considered part of the string
        // String.equals('\0') always returns false, even if String is empty
        // use length() == 0 to determine if a string is empty
        bool equals(char ch) const {
            if (length() != 1 || !ch) {
                return false;
            }
            return buffer()[0] == ch;
        }

        bool equals(const String &str) const {
            if (this == &str) {
                return true;
            }
            if (length() != str.length()) {
                return false;
            }
            return strcmp(buffer(), str.buffer()) == 0;
        }

        bool equals(const char *cStr) const {
            if (!cStr) {
                return false;
            }
            if (length() == 0) {
                return (*cStr == 0);
            }
            return strcmp(buffer(), cStr) == 0;
        }

        bool equals(const __FlashStringHelper *fStr) const {
            if (!fStr) {
                return false;
            }
            return strcmp_P(buffer(), reinterpret_cast<PGM_P>(fStr)) == 0;
        }

        // return true if String matches the end of str1
        // basically a.endsWith(b) = b.endEquals(a)
        //
        // String("123").endEquals("test123") == true
        // String("").endEquals(nullptr) == false
        bool endEquals(const char *str1) const {
            size_t len1;
            size_t len2;
            if (!str1 || (len2 = length()) > (len1 = strlen(str1))) {
                return false;
            }
            if (len2 == len1) {
                return strcmp(str1, buffer()) == 0;
            }
            return strcmp(str1 + len1 - len2, buffer()) == 0;
        }

        bool endEquals(const __FlashStringHelper *fStr) const {
            PGM_P str1 = reinterpret_cast<PGM_P>(fStr);
            size_t len1;
            size_t len2;
            if (!str1 || (len2 = length()) > (len1 = strlen_P(str1))) {
                return false;
            }
            if (len2 == len1) {
                return strcmp_P_P(str1, buffer()) == 0;
            }
            return strcmp_P_P(str1 + len1 - len2, buffer()) == 0;
        }

        // we can use endsWith in this case
        inline bool endEquals(const String &str) const {
            return str.endsWith(*this);
        }

        // return true if String matches the end of str1
        // basically a.endsWith(b) = b.endEquals(a)
        //
        // String("abc").endEqualsIgnoreCase("TESTABC") == true
        // String("").endEqualsIgnoreCase(nullptr) == false
        bool endEqualsIgnoreCase(const char *str1) const {
            size_t len1;
            size_t len2;
            if (!str1 || (len2 = length()) > (len1 = strlen(str1))) {
                return false;
            }
            if (len2 == len1) {
                return strcasecmp(str1, buffer()) == 0;
            }
            return strcasecmp(str1 + len1 - len2, buffer()) == 0;
        }

        bool endEqualsIgnoreCase(const __FlashStringHelper *fStr) const {
            PGM_P str1 = reinterpret_cast<PGM_P>(fStr);
            size_t len1;
            size_t len2;
            if (!str1 || (len2 = length()) > (len1 = strlen_P(str1))) {
                return false;
            }
            if (len2 == len1) {
                return strcasecmp_P_P(str1, buffer()) == 0;
            }
            return strcasecmp_P_P(str1 + len1 - len2, buffer()) == 0;
        }

        // we can just use endsWithIgnoreCase in this case
        inline bool endEqualsIgnoreCase(const String &str) const {
            return str.endsWithIgnoreCase(*this);
        }

        //
        // equals() with offset
        //

        unsigned char equals(const String &str, size_t offset) const {
            size_t len1;
            size_t len2;
            if (((len1 = length()) == 0) || (offset >= len1) || ((len2 = str.length()) == 0) || ((len1 + offset) != len2)) {
                return false;
            }
            return strcmp(buffer() + offset, str.c_str()) == 0;
        }

        unsigned char equals(const char *cStr, size_t offset) const {
            if (!cStr || !*cStr || offset >= length()) {
                return false;
            }
            return strcmp(buffer() + offset, cStr) == 0;
        }

        unsigned char equals(const __FlashStringHelper *fStr, size_t offset) const {
            if (!fStr || offset >= length()) {
                return false;
            }
            return strcmp_P(buffer() + offset, reinterpret_cast<PGM_P>(fStr)) == 0;
        }

        //
        // equalsIgnoreCase() with offset
        //

        unsigned char equalsIgnoreCase(const String &str, size_t offset = 0) const {
            if (offset == 0 && this == &str) {
                return true;
            }
            size_t len1;
            size_t len2;
            if (((len1 = length()) == 0) || (offset >= len1) || ((len2 = str.length()) == 0) || ((len1 + offset) != len2)) {
                return false;
            }
            return strcasecmp(buffer() + offset, str.c_str()) == 0;
        }

        unsigned char equalsIgnoreCase(const char *cStr, size_t offset = 0) const {
            if (!cStr || !*cStr || offset >= length()) {
                return false;
            }
            return strcasecmp(buffer() + offset, cStr) == 0;
        }

        unsigned char equalsIgnoreCase(const __FlashStringHelper *fStr, size_t offset = 0) const {
            if (!fStr || offset >= length()) {
                return false;
            }
            return strcasecmp_P(buffer() + offset, reinterpret_cast<PGM_P>(fStr)) == 0;
        }

    // additional startWith methods with ignorecase and support for char, const char *, const __FlashStringHelper * and String
    public:

        //
        // _startsWith
        //

        inline  unsigned char _startsWith(const char *prefix, size_t prefixLen, size_t offset) const {
            return prefixLen && (length() >= prefixLen + offset) && !strncmp(c_str() + offset, prefix, prefixLen);
        }
        inline unsigned char _startsWith_P(PGM_P prefix, size_t prefixLen, size_t offset) const {
            return prefixLen && (length() >= prefixLen + offset) && !strncmp_P(c_str() + offset, prefix, prefixLen);
        }
        inline unsigned char _startsWithIgnoreCase(const char *prefix, size_t prefixLen, size_t offset) const {
            return prefixLen && (length() >= prefixLen + offset) && !strncasecmp(c_str() + offset, prefix, prefixLen);
        }
        inline unsigned char _startsWithIgnoreCase_P(PGM_P prefix, size_t prefixLen, size_t offset) const {
            return prefixLen && (length() >= prefixLen + offset) && !strncasecmp_P(c_str() + offset, prefix, prefixLen);
        }

        //
        // startsWith(<char|const char *|const String &|const __FlashStringHelper *>[,offset]])
        //

        unsigned char startsWith(char ch) const {
            return (length() != 0) && (buffer()[0] == ch);
        }
        unsigned char startsWith(char ch, size_t offset) const {
            return (length() > offset) && (buffer()[offset] == ch);
        }
        unsigned char startsWith(const char *prefix, size_t offset = 0) const {
            return prefix && _startsWith(prefix, strlen(prefix), offset);
        }
        unsigned char startsWith(const String &prefix, size_t offset = 0) const {
            size_t len;
            return ((len = prefix.length())) != 0 && _startsWith(prefix.buffer(), len, offset);
        }
        unsigned char startsWith(const __FlashStringHelper *prefix, size_t offset = 0) const {
            return prefix && _startsWith_P(reinterpret_cast<PGM_P>(prefix), strlen_P(reinterpret_cast<PGM_P>(prefix)), offset);
        }

        //
        // startsWithIgnoreCase(<char|const char *|const String &|const __FlashStringHelper *>[,offset]])
        //

        unsigned char startsWithIgnoreCase(char ch, size_t offset = 0) const {
            return (length() > offset) && (tolower(buffer()[offset]) == tolower(ch));
        }
        unsigned char startsWithIgnoreCase(const char *prefix, size_t offset = 0) const {
            return prefix && _startsWithIgnoreCase(prefix, strlen(prefix), offset);
        }
        unsigned char startsWithIgnoreCase(const String &prefix, size_t offset = 0) const {
            size_t len;
            return ((len = prefix.length())) != 0 && _startsWithIgnoreCase(prefix.buffer(), len, offset);
        }
        unsigned char startsWithIgnoreCase(const __FlashStringHelper *prefix, size_t offset = 0) const {
            return prefix && _startsWithIgnoreCase_P(reinterpret_cast<PGM_P>(prefix), strlen_P(reinterpret_cast<PGM_P>(prefix)), offset);
        }

    protected:
        //
        // _endsWith
        //

        inline __attribute__((__always_inline__))
            unsigned char _endsWith(const char *suffix, size_t suffixLen) const {
            size_t len;
            return suffixLen && ((len = length()) >= suffixLen) && !strcmp(buffer() + len - suffixLen, suffix);
        }
        inline __attribute__((__always_inline__))
            unsigned char _endsWith_P(PGM_P suffix, size_t suffixLen) const {
            size_t len;
            return suffixLen && ((len = length()) >= suffixLen) && !strcmp_P(buffer() + len - suffixLen, suffix);
        }
        inline __attribute__((__always_inline__))
            unsigned char _endsWithIgnoreCase(const char *suffix, size_t suffixLen) const {
            size_t len;
            return suffixLen && ((len = length()) >= suffixLen) && !strcasecmp(buffer() + len - suffixLen, suffix);
        }
        inline __attribute__((__always_inline__))
            unsigned char _endsWithIgnoreCase_P(PGM_P suffix, size_t suffixLen) const {
            size_t len;
            return suffixLen && ((len = length()) >= suffixLen) && !strcasecmp_P(buffer() + len - suffixLen, suffix);
        }


    // additional endsWith methods with ignorecase and support for char, const char *, const __FlashStringHelper * and String
    public:
        //
        // endsWith
        //

        unsigned char endsWith(char ch) const {
            auto len = length();
            return (len != 0) && (buffer()[len - 1] == ch);
        }
        unsigned char endsWith(const String &suffix) const {
            return _endsWith(suffix.buffer(), suffix.length());
        }
        unsigned char endsWith(const char *suffix) const {
            return suffix && _endsWith(suffix, strlen(suffix));
        }
        unsigned char endsWith(const __FlashStringHelper *suffix) const {
            return suffix && _endsWith_P(reinterpret_cast<PGM_P>(suffix), strlen_P(reinterpret_cast<PGM_P>(suffix)));
        }

        //
        // endsWithIgnoreCase
        //

        unsigned char endsWithIgnoreCase(char ch) const {
            auto len = length();
            return (len != 0) && (tolower(buffer()[len - 1]) == tolower(ch));
        }
        unsigned char endsWithIgnoreCase(const String &suffix) const {
            return _endsWithIgnoreCase(suffix.buffer(), suffix.length());
        }
        unsigned char endsWithIgnoreCase(const char *suffix) const {
            return suffix && _endsWithIgnoreCase(suffix, strlen(suffix));
        }
        unsigned char endsWithIgnoreCase(const __FlashStringHelper *suffix) const {
            return suffix && _endsWithIgnoreCase_P(reinterpret_cast<PGM_P>(suffix), strlen_P(reinterpret_cast<PGM_P>(suffix)));
        }


        // // search
        // int indexOf(char ch, unsigned int fromIndex = 0) const;
        // int indexOf(const char *str, unsigned int fromIndex = 0) const;
        // int indexOf(const __FlashStringHelper *str, unsigned int fromIndex = 0) const {
        //     return indexOf((const char*)str, fromIndex);
        // }
        // int indexOf(const String &str, unsigned int fromIndex = 0) const;
        // int lastIndexOf(char ch) const;
        // int lastIndexOf(char ch, unsigned int fromIndex) const;
        // int lastIndexOf(const String &str) const;
        // int lastIndexOf(const String &str, unsigned int fromIndex) const;

    // internal search functions
    protected:
        // findLength is optional, provide only if available
        int _indexOf(const char *find, size_t fromIndex, size_t findLength = ~0) const
        {
            size_t len;
            if (!find || !findLength || ((len = length()) == 0) || (findLength != ~0U && (fromIndex + findLength >= len))) {
                return -1;
            }
            auto ptr = buffer();
            auto idxPtr = strstr(ptr + fromIndex, find);
            if (!idxPtr) {
                return -1;
            }
            return idxPtr - ptr;
        }

        int _indexOfIgnoreCase(const char *find, size_t fromIndex, size_t findLength = ~0) const
        {
            size_t len;
            if (!find || !findLength || ((len = length()) == 0) || (findLength != ~0U && (fromIndex + findLength >= len))) {
                return -1;
            }
            auto ptr = buffer();
            auto idxPtr = stristr(ptr + fromIndex, find);
            if (!idxPtr) {
                return -1;
            }
            return idxPtr - ptr;
        }

        int _indexOf_P(PGM_P find, size_t fromIndex, size_t findLength = ~0) const
        {
            size_t len;
            if (!find || !findLength || ((len = length()) == 0) || (findLength != ~0U && (fromIndex + findLength >= len))) {
                return -1;
            }
            auto ptr = buffer();
            auto idxPtr = strstr_P(ptr + fromIndex, find);
            if (!idxPtr) {
                return -1;
            }
            return idxPtr - ptr;
        }

        int _indexOfIgnoreCase_P(PGM_P find, size_t fromIndex, size_t findLength) const
        {
            size_t len;
            if (!find || !findLength || ((len = length()) == 0) || (fromIndex + findLength >= len)) {
                return -1;
            }
            auto ptr = buffer();
            auto idxPtr = stristr_P(const_cast<char *>(ptr) + fromIndex, find, findLength);
            if (!idxPtr) {
                return -1;
            }
            return idxPtr - ptr;
        }

        int _lastIndexOf(char find) const
        {
            auto ptr = strrchr(buffer(), find);
            if (!ptr) {
                return -1;
            }
            return ptr - buffer();
        }

        int _lastIndexOf_P(char find) const
        {
            auto ptr = strrchr_P(buffer(), find);
            if (!ptr) {
                return -1;
            }
            return ptr - buffer();
        }

        int _lastIndexOf(char find, size_t fromIndex) const
        {
            if (!find) {
                return -1;
            }
            auto len = length();
            if (fromIndex == ~0U) {
                fromIndex = len;
            }
            else if (fromIndex > len || fromIndex < 1) {
                return -1;
            }
            auto ptr = reinterpret_cast<const char *>(memrchr(buffer(), find, fromIndex));
            if (!ptr) {
                return -1;
            }
            return ptr - buffer();
        }

        int _lastIndexOf_P(char find, size_t fromIndex) const
        {
            if (!find) {
                return -1;
            }
            auto len = length();
            if (fromIndex == ~0U) {
                fromIndex = len;
            }
            else if (fromIndex > len || fromIndex < 1) {
                return -1;
            }
            auto ptr = reinterpret_cast<const char *>(memrchr(buffer(), find, fromIndex));
            if (!ptr) {
                return -1;
            }
            return ptr - buffer();
        }

        int _lastIndexOf_P(PGM_P find, size_t fromIndex, size_t findLen) const
        {
            size_t len;
            if (!find || !(len = length())) {
                return -1;
            }
            if (fromIndex == ~0U) {
                fromIndex = len;
            }
            else if (fromIndex < findLen || fromIndex > len) {
                return -1;
            }
            auto ptr = __strrstr_P(const_cast<char *>(buffer()), fromIndex + findLen, find, findLen);
            if (!ptr) {
                return -1;
            }
            return ptr - buffer();
        }

        int _lastIndexOf(const char *find, size_t fromIndex, size_t findLen) const
        {
            size_t len;
            if (!find || !(len = length())) {
                return -1;
            }
            if (fromIndex == ~0U) {
                fromIndex = len;
            }
            else if (fromIndex < findLen || fromIndex > len) {
                return -1;
            }
            auto ptr = __strrstr(const_cast<char *>(buffer()), fromIndex + findLen, find, findLen);
            if (!ptr) {
                return -1;
            }
            return ptr - buffer();
        }


    // additional indexOf methods with ignore case and support for char, const char *, const __FlashStringHelper * and String
    public:
        // search
        int indexOf(char ch, unsigned int fromIndex = 0) const;
        int indexOf(const String &str) const;
        int indexOf(const String &str, unsigned int fromIndex) const;

        int indexOf(const char *str, unsigned int fromIndex = 0) const {
            return _indexOf(str, fromIndex, ~0U);
        }
        int indexOf(const __FlashStringHelper *fstr, unsigned int fromIndex = 0) const {
            return _indexOf_P(reinterpret_cast<PGM_P>(fstr), fromIndex, ~0U);
        }


        int indexOfIgnoreCase(char ch, unsigned int fromIndex = 0) const {
            if (fromIndex >= len())
                return -1;
            const char *temp = strichr(buffer() + fromIndex, ch);
            if (temp == NULL)
                return -1;
            return temp - buffer();
        }


        int indexOfIgnoreCase(const char *str, unsigned int fromIndex = 0) const {
            return _indexOfIgnoreCase(str, fromIndex, ~0U);
        }
        int indexOfIgnoreCase(const String &str, unsigned int fromIndex = 0) const {
            return _indexOfIgnoreCase(str.c_str(), fromIndex, str.length());
        }
        int indexOfIgnoreCase(const __FlashStringHelper *fstr, unsigned int fromIndex = 0) const {
            return _indexOfIgnoreCase_P(reinterpret_cast<PGM_P>(fstr), fromIndex, ~0U);
        }


        int lastIndexOf(char ch) const {
            return _lastIndexOf(ch);
        }
        int lastIndexOf(char ch, unsigned int fromIndex) const {
            return _lastIndexOf(ch, fromIndex);
        }
        int lastIndexOf(const String &str) const {
            auto strLen = str.length();
            return _lastIndexOf(str.buffer(), length() - strLen, strLen);
        }
        int lastIndexOf(const String &str, unsigned int fromIndex) const {
            auto findLength = str.length();
            return _lastIndexOf(str.buffer(), fromIndex - findLength, findLength);
        }
        int lastIndexOf(const __FlashStringHelper *str, unsigned int fromIndex) const {
            auto findLength = strlen_P(reinterpret_cast<PGM_P>(str));
            return _lastIndexOf_P(reinterpret_cast<PGM_P>(str), fromIndex - findLength, findLength);
        }


    // new replace functions that support char, char *, const __FlashStringHelper * and String
    public:
        bool replace(char find, char replace);

    // internal replace function with PROGMEM support
    protected:
        bool _replace(PGM_P find, size_t findLen, PGM_P replace, size_t replaceLen);

    // public replace function with return type if bool for success
    public:
        inline bool replace(const char *find, const char *replace) {
            return _replace(find, strlen(find), replace, strlen(replace));
        }
        inline bool replace(const char *find, const __FlashStringHelper *replace) {
            return _replace(find, strlen(find), reinterpret_cast<PGM_P>(replace), strlen_P(reinterpret_cast<PGM_P>(replace)));
        }
        inline bool replace(const char *find, const String &replace) {
            return _replace(find, strlen(find), replace.buffer(), replace.length());
        }
        inline bool replace(const String &find, const String &replace) {
            return _replace(find.buffer(), find.length(), replace.buffer(), replace.length());
        }
        inline bool replace(const String &find, const char *replace) {
            return _replace(find.buffer(), find.length(), replace, strlen(replace));
        }
        inline bool replace(const String &find, const __FlashStringHelper *replace) {
            return _replace(find.buffer(), find.length(), reinterpret_cast<PGM_P>(replace), strlen_P(reinterpret_cast<PGM_P>(replace)));
        }
        inline bool replace(const __FlashStringHelper *find, const String &replace) {
            return _replace(reinterpret_cast<PGM_P>(find), strlen_P(reinterpret_cast<PGM_P>(find)), replace.buffer(), replace.length());
        }
        inline  bool replace(const __FlashStringHelper *find, const char *replace) {
            return _replace(reinterpret_cast<PGM_P>(find), strlen_P(reinterpret_cast<PGM_P>(find)), replace, strlen(replace));
        }
        inline  bool replace(const __FlashStringHelper *find, const __FlashStringHelper *replace) {
            return _replace(reinterpret_cast<PGM_P>(find), strlen_P(reinterpret_cast<PGM_P>(find)), reinterpret_cast<PGM_P>(replace), strlen_P(reinterpret_cast<PGM_P>(replace)));
        }

    // new trim function internal methods
    // trim, rtrim, ltrim
    // direct support for char, char *, const FlashStringHelper * and String as subset to trim
    protected:
        enum class TrimType : uint8_t {
            LEFT = 0x01,
            RIGHT = 0x02,
            BOTH = LEFT | RIGHT
        };

        String &_trim(TrimType type);

        inline String &_trim(TrimType type, char character) {
            char buf[2] = { character, 0 };
            return _trim(type, buf, 1);
        }

        String &_trim(TrimType type, PGM_P characters, size_t charLen);

        inline String &_trim(TrimType type, const char *characters) {
            return _trim(type, characters, strlen(characters));
        }

        inline String &_trim(TrimType type, const __FlashStringHelper *characters) {
            return _trim(type, reinterpret_cast<PGM_P>(characters), strlen_P(reinterpret_cast<PGM_P>(characters)));
        }

        inline String &_trim(TrimType type, const String &characters) {
            return _trim(type, characters.c_str(), characters.length());
        }

    // public trim functions with return type
    public:
        inline String &trim() {
            return _trim(TrimType::BOTH);
        }
        inline String &rtrim() {
            return _trim(TrimType::RIGHT);
        }
        inline String &ltrim() {
            return _trim(TrimType::LEFT);
        }

        inline String &trim(char character) {
            return _trim(TrimType::BOTH, character);
        }
        inline String &rtrim(char character) {
            return _trim(TrimType::RIGHT, character);
        }
        inline String &ltrim(char character) {
            return _trim(TrimType::LEFT, character);
        }

        inline String &trim(const char *characters) {
            return _trim(TrimType::BOTH, characters);
        }
        inline String &trim(const String &characters) {
            return _trim(TrimType::BOTH, characters);
        }
        inline String &trim(const __FlashStringHelper *characters) {
            return _trim(TrimType::BOTH, characters);
        }

        inline String &rtrim(const char *characters) {
            return _trim(TrimType::RIGHT, characters);
        }
        inline String &rtrim(const String &characters) {
            return _trim(TrimType::RIGHT, characters);
        }
        inline String &rtrim(const __FlashStringHelper *characters) {
            return _trim(TrimType::RIGHT, characters);
        }

        inline  String &ltrim(const char *characters) {
            return _trim(TrimType::LEFT, characters);
        }
        inline String &ltrim(const String &characters) {
            return _trim(TrimType::LEFT, characters);
        }
        inline String &ltrim(const __FlashStringHelper *characters) {
            return _trim(TrimType::LEFT, characters);
        }

    public:
        // access to SSO and internals

        // returns allocated memory
        inline size_t __getAllocSize() const {
#if defined(ESP8266)
            if (isSSO()) {
                return 0;
            }
            return capacity() + 1;
#else
            return (length() + 8) & ~7;
#endif
        }

        // returns memory usage
        inline size_t __getMemorySize() const {
            return sizeof(String) + __getAllocSize();
        }

        // get pointer to allocated memory and release ownership
        // capacity is the size of the allocated block
        // if no memory is allocated, it returns nullptr and capacity is set to 0
        // the string is empty afterwards
        inline char *__release(size_t &capacity) {
            if (isSSO()) {
                capacity = 0;
                return nullptr;
            }
            capacity = this->capacity() + 1;
            auto rPtr = ptr.buff;
            ptr = {};
            invalidate();
            return rPtr;
        }

        // see __release(size_t &capacity)
        inline char *__release() {
            if (isSSO()) {
                return nullptr;
            }
            auto rPtr = ptr.buff;
            ptr = {};
            invalidate();
            return rPtr;
        }
        void remove(unsigned int index);
        void remove(unsigned int index, unsigned int count);
        void toLowerCase(void);
        void toUpperCase(void);
        void trim(void);

        // parsing/conversion
        long toInt(void) const;
        float toFloat(void) const;
	double toDouble(void) const;

    protected:
        // Contains the string info when we're not in SSO mode
        struct _ptr {
            char *   buff;
            uint32_t cap;
            uint32_t len;
        };
        // This allows strings up up to 11 (10 + \0 termination) without any extra space.
        enum { SSOSIZE = sizeof(struct _ptr) + 4 - 1 }; // Characters to allocate space for SSO, must be 12 or more
        struct _sso {
            char     buff[SSOSIZE];
            unsigned char len   : 7; // Ensure only one byte is allocated by GCC for the bitfields
            unsigned char isSSO : 1;
        } __attribute__((packed)); // Ensure that GCC doesn't expand the flag byte to a 32-bit word for alignment issues
#ifdef BOARD_HAS_PSRAM
        enum { CAPACITY_MAX = 3145728 };
#else
        enum { CAPACITY_MAX = 65535 };
#endif
        union {
            struct _ptr ptr;
            struct _sso sso;
        };
        // Accessor functions
        inline bool isSSO() const { return sso.isSSO; }
        inline unsigned int len() const { return isSSO() ? sso.len : ptr.len; }
        inline unsigned int capacity() const { return isSSO() ? (unsigned int)SSOSIZE - 1 : ptr.cap; } // Size of max string not including terminal NUL
        inline void setSSO(bool set) { sso.isSSO = set; }
        inline void setLen(int len) {
            if (isSSO()) {
                sso.len = len;
                sso.buff[len] = 0;
            } else {
                ptr.len = len;
                if (ptr.buff) {
                    ptr.buff[len] = 0;
                }
            }
        }
        inline void setCapacity(int cap) { if (!isSSO()) ptr.cap = cap; }
        inline void setBuffer(char *buff) { if (!isSSO()) ptr.buff = buff; }
        // Buffer accessor functions
        inline const char *buffer() const { return (const char *)(isSSO() ? sso.buff : ptr.buff); }
        inline char *wbuffer() const { return isSSO() ? const_cast<char *>(sso.buff) : ptr.buff; } // Writable version of buffer

    protected:
        void init(void);
        void invalidate(void);
        unsigned char changeBuffer(unsigned int maxStrLen);
        unsigned char concat(const char *cstr, unsigned int length);

        // copy and move
        String & copy(const char *cstr, unsigned int length);
        String & copy(const __FlashStringHelper *pstr, unsigned int length);
#ifdef __GXX_EXPERIMENTAL_CXX0X__
        void move(String &rhs);
#endif
};

class StringSumHelper: public String {
    public:
        StringSumHelper(const String &s) :
                String(s) {
        }
        StringSumHelper(const char *p) :
                String(p) {
        }
        StringSumHelper(char c) :
                String(c) {
        }
        StringSumHelper(unsigned char num) :
                String(num) {
        }
        StringSumHelper(int num) :
                String(num) {
        }
        StringSumHelper(unsigned int num) :
                String(num) {
        }
        StringSumHelper(long num) :
                String(num) {
        }
        StringSumHelper(unsigned long num) :
                String(num) {
        }
        StringSumHelper(float num) :
                String(num) {
        }
        StringSumHelper(double num) :
                String(num) {
        }
};

extern const String emptyString;

#endif  // __cplusplus
#endif  // String_class_h
