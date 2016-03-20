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

/**
 * Find the length of a C string.
 *
 * @param [in] str  The string whose length we should find
 * @param [in] max  Maximum number of characters to search
 * @return Length of the string
 */
usize length(const char *str, usize max = -1);

/** Copy a string. */
char* copy(char* dst, const char* src, usize max);

/** Reverse a string. */
char* reverse(char* str, usize max);

/** Destructively convert an ASCII C String to uppercase. */
void uppercase(char *str);

/**
 * @defgroup To-String Converters
 */

/**
 * Convert a signed integer to a string.
 *
 * @param [in]     value   The value to convert
 * @param [in,out] buffer  Buffer to write to
 * @param [in]     length  Length of the buffer
 * @param [in]     base    Base to convert to. Default is base 10.
 * @param [in]     capitalized  Should the alphabetic digits be capitalized? Default is false.
 * @{
 */
char* fromInteger(i32 value, char* buffer, usize length, u8 base = 10, bool capitalized = false);
char* fromUnsignedInteger(u32 value, char* buffer, usize length, u8 base = 10, bool capitalized = false);
char* fromLongInteger(i64 value, char* buffer, usize length, u8 base = 10, bool capitalized = false);
char* fromUnsignedLongInteger(u64 value, char* buffer, usize length, u8 base = 10, bool capitalized = false);
/** @} */

/** Convert a bool to a string. */
char* fromBool(bool value, char* str, usize length);

/** @} */

} /* namespace CString */
} /* namespace kstd */
