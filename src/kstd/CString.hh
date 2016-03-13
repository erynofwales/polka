/* CString.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Utilities for dealing with C strings.
 */

#include <stddef.h>

namespace kstd {
namespace CString {

/** Find the length of a C string. */
size_t length(char *str);

/** Destructively convert an ASCII C String to uppercase. */
void uppercase(char *str);

} /* namespace CString */
} /* namespace kstd */
