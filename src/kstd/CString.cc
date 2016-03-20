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
length(char* str)
{
    usize i;
    for (i = 0; str[i] != '\0'; i++);
    return i;
}


char*
copy(char* dst,
     const char* src,
     usize length)
{
    for (usize i = 0; i < length; i++) {
        dst[i] = src[i];
        if (src[i] == '\0') {
            break;
        }
    }
    return dst;
}


void
uppercase(char *str)
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

char*
fromInteger(int value,
            char* str,
            usize length)
{
    return 0;
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
