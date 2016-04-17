/* Types.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * File description.
 */

#ifndef __KSTD_TYPES_HH__
#define __KSTD_TYPES_HH__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef __UINT8_TYPE__  u8;
typedef __INT8_TYPE__   i8;
typedef __UINT16_TYPE__ u16;
typedef __INT16_TYPE__  i16;
typedef __UINT32_TYPE__ u32;
typedef __INT32_TYPE__  i32;
typedef __UINT64_TYPE__ u64;
typedef __INT64_TYPE__  i64;
typedef __SIZE_TYPE__ usize;

#if __WORDSIZE == 64
typedef i64 iptr;
typedef u64 uptr;
#else
typedef i32 iptr;
typedef u32 uptr;
#endif

#ifdef __cplusplus
}
#endif

#endif /* __KSTD_TYPES_HH__ */
