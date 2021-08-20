/*
 WString.cpp - String library for Wiring & Arduino
 ...mostly rewritten by Paul Stoffregen...
 Copyright (c) 2009-10 Hernando Barragan.  All rights reserved.
 Copyright 2011, Paul Stoffregen, paul@pjrc.com
 Modified by Ivan Grokhotkov, 2014 - esp8266 support
 Modified by Michael C. Miller, 2015 - esp8266 progmem support

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

#include <Arduino.h>
#include "WString.h"
#include "stdlib_noniso.h"

/*********************************************/
/*  Constructors                             */
/*********************************************/

String::String(const char *cstr) {
    init();
    if (cstr)
        copy(cstr, strlen(cstr));
}

String::String(const String &value) {
    init();
    *this = value;
}

String::String(const __FlashStringHelper *pstr) {
    init();
    *this = pstr; // see operator =
}

#ifdef __GXX_EXPERIMENTAL_CXX0X__
String::String(String &&rval) {
    init();
    move(rval);
}

String::String(StringSumHelper &&rval) {
    init();
    move(rval);
}
#endif

String::String(char c) {
    init();
    char buf[] = { c, '\0' };
    *this = buf;
}

String::String(unsigned char value, unsigned char base) {
    init();
    char buf[1 + 8 * sizeof(unsigned char)];
    utoa(value, buf, base);
    *this = buf;
}

String::String(int value, unsigned char base) {
    init();
    char buf[2 + 8 * sizeof(int)];
    if (base == 10) {
        sprintf(buf, "%d", value);
    } else {
        itoa(value, buf, base);
    }
    *this = buf;
}

String::String(unsigned int value, unsigned char base) {
    init();
    char buf[1 + 8 * sizeof(unsigned int)];
    utoa(value, buf, base);
    *this = buf;
}

String::String(long value, unsigned char base) {
    init();
    char buf[2 + 8 * sizeof(long)];
    if (base==10) {
        sprintf(buf, "%ld", value);
    } else {
        ltoa(value, buf, base);
    }
    *this = buf;
}

String::String(unsigned long value, unsigned char base) {
    init();
    char buf[1 + 8 * sizeof(unsigned long)];
    ultoa(value, buf, base);
    *this = buf;
}

String::String(long long value) {
    init();
    char buf[std::numeric_limits<decltype(value)>::digits10 + 2];
    sprintf(buf, "%lld", value);
    *this = buf;
}

String::String(unsigned long long value) {
    init();
    char buf[std::numeric_limits<decltype(value)>::digits10 + 2];
    sprintf(buf, "%llu", value);
    *this = buf;
}

// String::String(long long value, unsigned char base) {
//     init();
//     char buf[1 + 8 * sizeof(value)];
//     *this = lltoa(value, buf, sizeof(buf), base);
// }

// String::String(unsigned long long value, unsigned char base) {
//     init();
//     char buf[1 + 8 * sizeof(value)];
//     *this = ulltoa(value, buf, sizeof(buf), base);
// }

String::String(float value, unsigned char decimalPlaces) {
    init();
    char buf[33];
    *this = dtostrf(value, (decimalPlaces + 2), decimalPlaces, buf);
}

String::String(double value, unsigned char decimalPlaces) {
    init();
    char buf[33];
    *this = dtostrf(value, (decimalPlaces + 2), decimalPlaces, buf);
}

/*********************************************/
/*  Memory Management                        */
/*********************************************/

void String::init(void) {
    setSSO(false);
    setBuffer(nullptr);
    setCapacity(0);
    setLen(0);
}

void String::invalidate(void) {
    if(!isSSO() && wbuffer())
        free(wbuffer());
    init();
}

bool String::reserve(unsigned int size) {
    if(buffer() && capacity() >= size)
        return 1;
    if(changeBuffer(size)) {
        if(len() == 0)
            wbuffer()[0] = 0;
        return 1;
    }
    return 0;
}

unsigned char String::changeBuffer(unsigned int maxStrLen) {
    // Can we use SSO here to avoid allocation?
    if (maxStrLen < sizeof(sso.buff) - 1) {
        if (isSSO() || !buffer()) {
            // Already using SSO, nothing to do
	    uint16_t oldLen = len();
            setSSO(true);
            setLen(oldLen);
            return 1;
        } else { // if bufptr && !isSSO()
            // Using bufptr, need to shrink into sso.buff
            char temp[sizeof(sso.buff)];
            memcpy(temp, buffer(), maxStrLen);
            free(wbuffer());
            uint16_t oldLen = len();
            setSSO(true);
            memcpy(wbuffer(), temp, maxStrLen);
            setLen(oldLen);
            return 1;
        }
    }
    // Fallthrough to normal allocator
    size_t newSize = (maxStrLen + 16) & (~0xf);
    // Make sure we can fit newsize in the buffer
    if (newSize > CAPACITY_MAX) {
        return false;
    }
    uint16_t oldLen = len();
    char *newbuffer = (char *) realloc(isSSO() ? nullptr : wbuffer(), newSize);
    if (newbuffer) {
        size_t oldSize = capacity() + 1; // include NULL.
        if (isSSO()) {
            // Copy the SSO buffer into allocated space
            memmove(newbuffer, sso.buff, sizeof(sso.buff));
        }
        if (newSize > oldSize)
        {
            memset(newbuffer + oldSize, 0, newSize - oldSize);
        }
        setSSO(false);
        setCapacity(newSize - 1);
        setBuffer(newbuffer);
        setLen(oldLen); // Needed in case of SSO where len() never existed
        return 1;
    }
    return 0;
}

// /*********************************************/
// /*  Copy and Move                            */
// /*********************************************/

String & String::copy(const char *cstr, unsigned int length) {
    if(!reserve(length)) {
        invalidate();
        return *this;
    }
    memmove(wbuffer(), cstr, length + 1);
    setLen(length);
    return *this;
}

String & String::copy(const __FlashStringHelper *pstr, unsigned int length) {
    if (!reserve(length)) {
        invalidate();
        return *this;
    }
    memcpy_P(wbuffer(), (PGM_P)pstr, length + 1); // We know wbuffer() cannot ever be in PROGMEM, so memcpy safe here
    setLen(length);
    return *this;
}

#ifdef __GXX_EXPERIMENTAL_CXX0X__
void String::move(String &rhs) {
    if(buffer()) {
        if(capacity() >= rhs.len()) {
            memmove(wbuffer(), rhs.buffer(), rhs.length() + 1);
            setLen(rhs.len());
	    rhs.invalidate();
            return;
        } else {
            if (!isSSO()) {
                free(wbuffer());
                setBuffer(nullptr);
            }
        }
    }
    if (rhs.isSSO()) {
        setSSO(true);
        memmove(sso.buff, rhs.sso.buff, sizeof(sso.buff));
    } else {
        setSSO(false);
        setBuffer(rhs.wbuffer());
    }
    setCapacity(rhs.capacity());
    setLen(rhs.len());
    rhs.setSSO(false);
    rhs.setCapacity(0);
    rhs.setBuffer(nullptr);
    rhs.setLen(0);
}
#endif

String & String::operator =(const String &rhs) {
    if(this == &rhs)
        return *this;

    if(rhs.buffer())
        copy(rhs.buffer(), rhs.len());
    else
        invalidate();

    return *this;
}

#ifdef __GXX_EXPERIMENTAL_CXX0X__
String & String::operator =(String &&rval) {
    if(this != &rval)
        move(rval);
    return *this;
}

String & String::operator =(StringSumHelper &&rval) {
    if(this != &rval)
        move(rval);
    return *this;
}
#endif

String & String::operator =(const char *cstr) {
    if(cstr)
        copy(cstr, strlen(cstr));
    else
        invalidate();

    return *this;
}

String & String::operator =(const __FlashStringHelper *pstr) {
    if(pstr)
        copy(pstr, strlen_P((PGM_P)pstr));
    else
        invalidate();

    return *this;
}

// /*********************************************/
// /*  concat                                   */
// /*********************************************/

unsigned char String::concat(const String &s) {
    // Special case if we're concatting ourself (s += s;) since we may end up
    // realloc'ing the buffer and moving s.buffer in the method called
    if (&s == this) {
        unsigned int newlen = 2 * len();
        if (!s.buffer())
            return 0;
        if (s.len() == 0)
            return 1;
        if (!reserve(newlen))
            return 0;
        memmove(wbuffer() + len(), buffer(), len());
        setLen(newlen);
        wbuffer()[len()] = 0;
        return 1;
    } else {
        return concat(s.buffer(), s.len());
    }
}

unsigned char String::concat(const char *cstr, unsigned int length) {
    unsigned int newlen = len() + length;
    if(!cstr)
        return 0;
    if(length == 0)
        return 1;
    if(!reserve(newlen))
        return 0;
    if (cstr >= wbuffer() && cstr < wbuffer() + len())
        // compatible with SSO in ram #6155 (case "x += x.c_str()")
        memmove(wbuffer() + len(), cstr, length + 1);
    else
        // compatible with source in flash #6367
        memcpy_P(wbuffer() + len(), cstr, length + 1);
    setLen(newlen);
    return 1;
}

unsigned char String::concat(const char *cstr) {
    if(!cstr)
        return 0;
    return concat(cstr, strlen(cstr));
}

unsigned char String::concat(char c) {
    char buf[] = { c, '\0' };
    return concat(buf, 1);
}

unsigned char String::concat(unsigned char num) {
    char buf[1 + 3 * sizeof(unsigned char)];
    return concat(buf, sprintf(buf, "%d", num));
}

unsigned char String::concat(int num) {
    char buf[2 + 3 * sizeof(int)];
    return concat(buf, sprintf(buf, "%d", num));
}

unsigned char String::concat(unsigned int num) {
    char buf[1 + 3 * sizeof(unsigned int)];
    utoa(num, buf, 10);
    return concat(buf, strlen(buf));
}

unsigned char String::concat(long num) {
    char buf[2 + 3 * sizeof(long)];
    return concat(buf, sprintf(buf, "%ld", num));
}

unsigned char String::concat(unsigned long num) {
    char buf[1 + 3 * sizeof(unsigned long)];
    ultoa(num, buf, 10);
    return concat(buf, strlen(buf));
}

unsigned char String::concat(float num) {
    char buf[20];
    char* string = dtostrf(num, 4, 2, buf);
    return concat(string, strlen(string));
}

unsigned char String::concat(double num) {
    char buf[20];
    char* string = dtostrf(num, 4, 2, buf);
    return concat(string, strlen(string));
}

unsigned char String::concat(const __FlashStringHelper * str) {
    if (!str) return 0;
    int length = strlen_P((PGM_P)str);
    if (length == 0) return 1;
    unsigned int newlen = len() + length;
    if (!reserve(newlen)) return 0;
    memcpy_P(wbuffer() + len(), (PGM_P)str, length + 1);
    setLen(newlen);
    return 1;
}

/*********************************************/
/*  Concatenate                              */
/*********************************************/

StringSumHelper & operator +(const StringSumHelper &lhs, const String &rhs) {
    StringSumHelper &a = const_cast<StringSumHelper&>(lhs);
    if(!a.concat(rhs.buffer(), rhs.len()))
        a.invalidate();
    return a;
}

StringSumHelper & operator +(const StringSumHelper &lhs, const char *cstr) {
    StringSumHelper &a = const_cast<StringSumHelper&>(lhs);
    if(!cstr || !a.concat(cstr, strlen(cstr)))
        a.invalidate();
    return a;
}

StringSumHelper & operator +(const StringSumHelper &lhs, char c) {
    StringSumHelper &a = const_cast<StringSumHelper&>(lhs);
    if(!a.concat(c))
        a.invalidate();
    return a;
}

StringSumHelper & operator +(const StringSumHelper &lhs, unsigned char num) {
    StringSumHelper &a = const_cast<StringSumHelper&>(lhs);
    if(!a.concat(num))
        a.invalidate();
    return a;
}

StringSumHelper & operator +(const StringSumHelper &lhs, int num) {
    StringSumHelper &a = const_cast<StringSumHelper&>(lhs);
    if(!a.concat(num))
        a.invalidate();
    return a;
}

StringSumHelper & operator +(const StringSumHelper &lhs, unsigned int num) {
    StringSumHelper &a = const_cast<StringSumHelper&>(lhs);
    if(!a.concat(num))
        a.invalidate();
    return a;
}

StringSumHelper & operator +(const StringSumHelper &lhs, long num) {
    StringSumHelper &a = const_cast<StringSumHelper&>(lhs);
    if(!a.concat(num))
        a.invalidate();
    return a;
}

StringSumHelper & operator +(const StringSumHelper &lhs, unsigned long num) {
    StringSumHelper &a = const_cast<StringSumHelper&>(lhs);
    if(!a.concat(num))
        a.invalidate();
    return a;
}

StringSumHelper & operator +(const StringSumHelper &lhs, float num) {
    StringSumHelper &a = const_cast<StringSumHelper&>(lhs);
    if(!a.concat(num))
        a.invalidate();
    return a;
}

StringSumHelper & operator +(const StringSumHelper &lhs, double num) {
    StringSumHelper &a = const_cast<StringSumHelper&>(lhs);
    if(!a.concat(num))
        a.invalidate();
    return a;
}

StringSumHelper & operator + (const StringSumHelper &lhs, const __FlashStringHelper *rhs)
{
    StringSumHelper &a = const_cast<StringSumHelper&>(lhs);
    if (!a.concat(rhs))
        a.invalidate();
    return a;
}

// /*********************************************/
// /*  Comparison                               */
// /*********************************************/

int String::compareTo(const String &s) const {
    if(!buffer() || !s.buffer()) {
        if(s.buffer() && s.len() > 0)
            return 0 - *(unsigned char *) s.buffer();
        if(buffer() && len() > 0)
            return *(unsigned char *) buffer();
        return 0;
    }
    return strcmp(buffer(), s.buffer());
}

// bool String::equals(const String &s2) const {
//     return (len() == s2.len() && compareTo(s2) == 0);
// }

// bool String::equals(const char *cstr) const {
//     if(len() == 0)
//         return (cstr == NULL || *cstr == 0);
//     if(cstr == NULL)
//         return buffer()[0] == 0;
//     return strcmp(buffer(), cstr) == 0;
// }

bool String::equalsConstantTime(const String &s2) const {
    // To avoid possible time-based attacks present function
    // compares given strings in a constant time.
    if(len() != s2.len())
        return 0;
    //at this point lengths are the same
    if(len() == 0)
        return 1;
    //at this point lengths are the same and non-zero
    const char *p1 = buffer();
    const char *p2 = s2.buffer();
    unsigned int equalchars = 0;
    unsigned int diffchars = 0;
    while(*p1) {
        if(*p1 == *p2)
            ++equalchars;
        else
            ++diffchars;
        ++p1;
        ++p2;
    }
    //the following should force a constant time eval of the condition without a compiler "logical shortcut"
    unsigned char equalcond = (equalchars == len());
    unsigned char diffcond = (diffchars == 0);
    return (equalcond & diffcond); //bitwise AND
}

/*********************************************/
/*  Character Access                         */
/*********************************************/

// char String::charAt(unsigned int loc) const {
//     return operator[](loc);
// }

void String::setCharAt(unsigned int loc, char c) {
    if(loc < len())
        wbuffer()[loc] = c;
}

char & String::operator[](unsigned int index) {
    static char dummy_writable_char;
    if(index >= len() || !buffer()) {
        dummy_writable_char = 0;
        return dummy_writable_char;
    }
    return wbuffer()[index];
}

char String::operator[](unsigned int index) const {
    if(index >= len() || !buffer())
        return 0;
    return buffer()[index];
}

void String::getBytes(unsigned char *buf, unsigned int bufsize, unsigned int index) const {
    if(!bufsize || !buf)
        return;
    if(index >= len()) {
        buf[0] = 0;
        return;
    }
    unsigned int n = bufsize - 1;
    if(n > len() - index)
        n = len() - index;
    strncpy((char *) buf, buffer() + index, n);
    buf[n] = 0;
}

/*********************************************/
/*  Search                                   */
/*********************************************/

int String::indexOf(char ch, unsigned int fromIndex) const {
    if (fromIndex >= len())
        return -1;
    const char *temp = strchr(buffer() + fromIndex, ch);
    if (temp == NULL)
        return -1;
    return temp - buffer();
}

// int String::indexOf(const String &s2) const {
//     return indexOf(s2, 0);
// }

// int String::indexOf(const String &s2, unsigned int fromIndex) const {
//     if (fromIndex >= len())
//         return -1;
//     const char *found = strstr(buffer() + fromIndex, s2.buffer());
//     if (found == NULL)
//         return -1;
//     return found - buffer();
// }

// int String::lastIndexOf(char theChar) const {
//     return lastIndexOf(theChar, len() - 1);
// }

// int String::lastIndexOf(char ch, unsigned int fromIndex) const {
//     if(fromIndex >= len())
//         return -1;
//     char tempchar = buffer()[fromIndex + 1];
//     wbuffer()[fromIndex + 1] = '\0';
//     char* temp = strrchr(wbuffer(), ch);
//     wbuffer()[fromIndex + 1] = tempchar;
//     if(temp == NULL)
//         return -1;
//     return temp - buffer();
// }

// int String::lastIndexOf(const String &s2) const {
//     return lastIndexOf(s2, len() - s2.len());
// }

// int String::lastIndexOf(const String &s2, unsigned int fromIndex) const {
//     if(s2.len() == 0 || len() == 0 || s2.len() > len())
//         return -1;
//     if(fromIndex >= len())
//         fromIndex = len() - 1;
//     int found = -1;
//     for(char *p = wbuffer(); p <= wbuffer() + fromIndex; p++) {
//         p = strstr(p, s2.buffer());
//         if(!p)
//             break;
//         if((unsigned int) (p - wbuffer()) <= fromIndex)
//             found = p - buffer();
//     }
//     return found;
// }

String String::substring(unsigned int left, unsigned int right) const {
    if(left > right) {
        unsigned int temp = right;
        right = left;
        left = temp;
    }
    String out;
    if(left >= len())
        return out;
    if(right > len())
        right = len();
    char temp = buffer()[right];  // save the replaced character
    wbuffer()[right] = '\0';
    out = wbuffer() + left;  // pointer arithmetic
    wbuffer()[right] = temp;  //restore character
    return out;
}

/*********************************************/
/*  Modification                             */
/*********************************************/


void String::remove(unsigned int index, unsigned int count) {
    if(index >= len()) {
        return;
    }
    if(count <= 0) {
        return;
    }
    if(count > len() - index) {
        count = len() - index;
    }
    char *writeTo = wbuffer() + index;
    unsigned int newlen = len() - count;
    memmove(writeTo, wbuffer() + index + count, newlen - index);
    setLen(newlen);
    wbuffer()[newlen] = 0;
}

bool String::_replace(PGM_P find, size_t findLen, PGM_P replace, size_t replaceLen)
{
    if (length() == 0 || findLen == 0 || !find) {
        return false;
    }
    int diff = replaceLen - findLen;
    char *readFrom = wbuffer();
    char *foundAt;
    if (diff == 0) {
        while ((foundAt = strstr_P(readFrom, find)) != nullptr) {
            memmove(foundAt, replace, replaceLen);
            readFrom = foundAt + replaceLen;
        }
    }
    else if (diff < 0) {
        char *writeTo = wbuffer();
        while ((foundAt = strstr_P(readFrom, find)) != nullptr) {
            unsigned int n = foundAt - readFrom;
            memmove(writeTo, readFrom, n);
            writeTo += n;
            memmove(writeTo, replace, replaceLen);
            writeTo += replaceLen;
            readFrom = foundAt + findLen;
            setLen(len() + diff);
        }
        memmove(writeTo, readFrom, strlen(readFrom) + 1);
    }
    else {
        unsigned int size = len(); // compute size needed for result
        while ((foundAt = strstr_P(readFrom, find)) != nullptr) {
            readFrom = foundAt + findLen;
            size += diff;
        }
        if (size == len()) {
            return true;
        }
        if (size > capacity() && !changeBuffer(size)) {
            return false;
        }
        int index = len() - 1;
        while (index >= 0 && (index = lastIndexOf(find, index)) >= 0) {
            readFrom = wbuffer() + index + findLen;
            memmove(readFrom + diff, readFrom, len() - (readFrom - buffer()));
            int newLen = len() + diff;
            memmove(wbuffer() + index, replace, replaceLen);
            setLen(newLen);
            wbuffer()[newLen] = 0;
            index--;
        }
    }
    return true;
}

// void String::remove(unsigned int index, unsigned int count) {
//     if (index >= len()) {
//         return;
//     }
//     if (count <= 0) {
//         return;
//     }
//     if (count > len() - index) {
//         count = len() - index;
//     }
//     char *writeTo = wbuffer() + index;
//     unsigned int newlen = len() - count;
//     setLen(newlen);
//     memmove(writeTo, wbuffer() + index + count, newlen - index);
//     wbuffer()[newlen] = 0;
// }

String &String::toLowerCase(void) {
    if (!buffer())
        return *this;
    for (char *p = wbuffer(); *p; p++) {
        *p = tolower(*p);
    }
    return *this;
}

String &String::toUpperCase(void) {
    if (!buffer())
        return *this;
    for (char *p = wbuffer(); *p; p++) {
        *p = toupper(*p);
    }
    return *this;
}

// new trim functions

String &String::_trim(TrimType type)
{
    auto len = length();
    if (len == 0) {
        return *this;
    }
    char *begin = wbuffer();
    if (static_cast<uint8_t>(type) & static_cast<uint8_t>(TrimType::LEFT)) {
        while (isspace(*begin)) {
            begin++;
        }
    }
    char *end = wbuffer() + len - 1;
    if (static_cast<uint8_t>(type) & static_cast<uint8_t>(TrimType::RIGHT)) {
        while (isspace(*end) && end >= begin) {
            end--;
        }
    }
    unsigned int newlen = end + 1 - begin;
    if (begin > buffer()) {
        memmove(wbuffer(), begin, newlen);
    }
    setLen(newlen);
    wbuffer()[newlen] = 0;
    return *this;
}

String &String::_trim(TrimType type, PGM_P characters, size_t charLen)
{
    auto len = length();
    if (!len || !characters || !charLen) {
        return *this;
    }
    charLen++; // use memchr instead of strchr to avoid additional NUL byte check
    if (static_cast<uint8_t>(type) & static_cast<uint8_t>(TrimType::RIGHT)) {
        while (len && memchr(characters, buffer()[len - 1], charLen)) {
            len--;
        }
        setLen(len);
    }

    if (static_cast<uint8_t>(type) & static_cast<uint8_t>(TrimType::LEFT)) {
        size_t remove = 0;
        while (memchr(characters, buffer()[remove], charLen)) {
            remove++;
        }
        this->remove(0, remove); // remove adds NUL byte
    }
    else {
        wbuffer()[len] = 0;
    }
    return *this;
}

/*********************************************/
/*  Parsing / Conversion                     */
/*********************************************/

long String::toInt(void) const {
    if (buffer())
        return atol(buffer());
    return 0;
}

float String::toFloat(void) const {
    if (buffer())
        return atof(buffer());
    return 0;
}

double String::toDouble(void) const
{
    if (buffer())
        return atof(buffer());
    return 0.0;
}

// global empty string to allow returning const String& with nothing

const String emptyString;
