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
    while (p < t) {
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
_doConvertFromInteger(u64 value,
                      char* buffer,
                      usize length,
                      u8 base,
                      bool capitalized)
{
    static const char* digitsLower = "0123456789abcdefghijklmnopqrstuvwxyz";
    static const char* digitsUpper = "0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    const char* digits = capitalized ? digitsUpper : digitsLower;

    if (length == 0) {
        return 0;
    }
    if (value == 0) {
        buffer[0] = digits[0];
        return 1;
    }

    u8 place;
    usize i;
    for (i = 0; i < length && value != 0; i++) {
        place = value % base;
        buffer[i] = digits[place];
        value /= base;
    }
    return i;
}


usize
fromInteger(i64 value,
            char* buffer,
            usize length,
            u8 base,
            bool capitalized)
{
    if (base < 2 || base > 36) {
        return 0;
    }

    const bool negative = base == 10 && value < 0;
    if (negative) {
        value *= -1;
    }

    usize convertedLength  = _doConvertFromInteger(value, buffer, length, base, capitalized);

    if (convertedLength < length && negative) {
        buffer[convertedLength++] = '-';
    }
    if (convertedLength < length) {
        buffer[convertedLength] = '\0';
    }

    reverse(buffer, convertedLength);

    return convertedLength;
}


usize
fromUnsignedInteger(u64 value,
                    char* buffer,
                    usize length,
                    u8 base,
                    bool capitalized)
{
    if (base < 2 || base > 36) {
        return 0;
    }

    usize convertedLength = _doConvertFromInteger(value, buffer, length, base, capitalized);
    if (convertedLength < length) {
        buffer[convertedLength] = '\0';
    }
    reverse(buffer, convertedLength);
    return convertedLength;
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
