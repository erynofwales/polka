/* PrintFormat.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Declares printFormat(), for writing formatted strings to the appropriate channel.
 */

#include <stdarg.h>

namespace kstd {

/**
 * Write a format string to the appropriate output channel.
 * @return Number of characters printed
 * @{
 */
int printFormat(const char* format, ...) PRINTF(1,2);
int printFormat(const char* format, va_list args);
/** @} */

} /* namespace kstd */
