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

size_t
length(char *str)
{
    size_t i;
    for (i = 0; str[i] != '\0'; i++) { }
    return i;
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

} /* namespace CString */
} /* namespace kernel */
