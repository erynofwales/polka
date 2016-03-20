/* CString.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Utilities for dealing with C strings.
 */

#include "Types.hh"

namespace kstd {
namespace CString {

/** Find the length of a C string. */
usize length(char *str);

/** Copy a string. */
char* copy(char* dst, const char* src, usize length);

/** Destructively convert an ASCII C String to uppercase. */
void uppercase(char *str);

/**
 * @defgroup To-String Converters
 */

/** Convert an integer to a string. */
char* fromInteger(int value, char *str, usize length);

/** Convert a bool to a string. */
char* fromBool(bool value, char *str, usize length);

/** @} */

} /* namespace CString */
} /* namespace kstd */
