/* PrintFormat.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Declares printFormat(), for writing formatted strings to the appropriate channel.
 */

#ifndef __KSTD_PRINTFORMAT_HH__
#define __KSTD_PRINTFORMAT_HH__

#include <stdarg.h>
#include "Attributes.hh"

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

#endif /* __KSTD_PRINTFORMAT_HH__ */
