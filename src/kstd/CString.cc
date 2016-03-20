/* CString.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Implementation of some utilities for dealing with C strings.
 */

#include "CString.hh"
#include "ASCII.hh"

namespace kstd {
namespace CString {

usize
length(const char* str,
       usize max)
{
    usize i;
    for (i = 0; str[i] != '\0' && i < max; i++);
    return i;
}


char*
copy(char* dst,
     const char* src,
     usize max)
{
    usize len = length(src, max);
    for (usize i = 0; i < len; i++) {
        dst[i] = src[i];
        if (src[i] == '\0') {
            break;
        }
    }
    return dst;
}


char*
reverse(char* str,
        usize max)
{
    max = length(str, max);
    char* p = str;
    char* t = str + max - 1;
    char c;
    while (t > p) {
        c = *p;
        *p++ = *t;
        *t-- = c;
    }
    return str;
}


void
uppercase(char* str)
{
    for (char *p = str; *p != '\0'; p++) {
        if (Char::isLower(*p)) {
            *p = Char::toUpper(*p);
        }
    }
}

/*
 * Converters
 */

/**
 * Converts `value` to an un-reversed CString without a null terminator. It is
 * expected that callers of this function will append a null terminator and
 * reverse `str` at the appropriate time.
 *
 * @return A pointer to the character *after* all converted characters, such
 * that `p - buffer` is the number of characters converted.
 */
static usize
_doConvertFromInteger(u32 value,
                      char* buffer,
                      usize length,
                      u8 base,
                      bool capitalized)
{
    char* p = buffer;
    int place;
    do {
        if (usize(p - buffer) >= length) {
            break;
        }
        place = value % base;
        value /= base;
        *p++ = (place < 10) ? place + '0' : (place - 10) + (capitalized ? 'A' : 'a');
    } while (value != 0);

    return p - buffer;
}

char*
fromInteger(i32 value,
            char* buffer,
            usize length,
            u8 base,
            bool capitalized)
{
    const bool negative = base == 10 && value < 0;
    if (negative) {
        value *= -1;
    }

    usize convertedLength = _doConvertFromInteger(u32(value), buffer, length, base, capitalized);

    if (convertedLength < length && negative) {
        buffer[convertedLength++] = '-';
    }
    if (convertedLength < length) {
        buffer[convertedLength] = '\0';
    }

    reverse(buffer, convertedLength);

    return buffer;
}


char*
fromUnsignedInteger(u32 value,
                    char* buffer,
                    usize length,
                    u8 base,
                    bool capitalized)
{
    usize convertedLength = _doConvertFromInteger(u32(value), buffer, length, base, capitalized);
    if (convertedLength < length) {
        buffer[convertedLength] = '\0';
    }
    reverse(buffer, convertedLength);
    return buffer;
}


char*
fromLongInteger(i64 value,
                char* buffer,
                usize length,
                u8 base,
                bool capitalized)
{
    const bool negative = base == 10 && value < 0;
    if (negative) {
        value *= -1;
    }

    // Do large ints in two passes. First do the high-order bits, then the lower order bits.
    usize convertedLength;
    convertedLength  = _doConvertFromInteger(u32(value >> 32), buffer, length, base, capitalized);
    convertedLength += _doConvertFromInteger(u32(value), &buffer[convertedLength], length - convertedLength, base, capitalized);

    if (convertedLength < length && negative) {
        buffer[convertedLength++] = '-';
    }
    if (convertedLength < length) {
        buffer[convertedLength] = '\0';
    }

    reverse(buffer, convertedLength);

    return buffer;
}


char*
fromUnsignedLongInteger(u64 value,
                        char* buffer,
                        usize length,
                        u8 base,
                        bool capitalized)
{
    // Do large ints in two passes. First do the high-order bits, then the lower order bits.
    usize convertedLength;
    convertedLength  = _doConvertFromInteger(u32(value >> 32), buffer, length, base, capitalized);
    convertedLength += _doConvertFromInteger(u32(value), &buffer[convertedLength], length - convertedLength, base, capitalized);
    if (convertedLength < length) {
        buffer[convertedLength] = '\0';
    }
    reverse(buffer, convertedLength);
    return buffer;
}


char*
fromBool(bool value,
         char* str,
         usize length)
{
    copy(str, value ? "true" : "false", length);
    return str;
}

} /* namespace CString */
} /* namespace kernel */
