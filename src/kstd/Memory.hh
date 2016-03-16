/* Memory.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Basic, low-level memory utilities.
 */

#ifndef __MEMORY_HH__
#define __MEMORY_HH__

namespace kstd {
namespace Memory {

/** Copy `length` bytes from `from` to `to`. */
void *copy(void *to, const void *from, size_t length);

/** Move `length` bytes from `from` to `to`. */
void *move(void *to, const void *from, size_t length);

/** Set `length` bytes starting at `s` to zero. */
void *zero(void *s, size_t length);

/** Set `length` bytes starting at `s` to `value`. */
void *set(void *s, uint8_t value, size_t length); 
    
} /* namespace Memory */
} /* namespace kstd */

#endif /* __MEMORY_HH__ */
