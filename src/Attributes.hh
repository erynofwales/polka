/* Attributes.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Defines for common compiler attributes.
 *
 * See https://gcc.gnu.org/onlinedocs/gcc/Type-Attributes.html for GCC
 * attributes.
 */

#ifndef __ATTRIBUTES_HH__
#define __ATTRIBUTES_HH__

#define PACKED __attribute__((packed))
#define NORETURN __attribute__((noreturn))
#define PRINTF(formatArg, variableArgStart) __attribute__((format (printf, formatArg, variableArgStart)))

#endif /* __ATTRIBUTES_HH__ */
