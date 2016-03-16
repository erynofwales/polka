/* Memory.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Basic, low-level memory utilities.
 */

namespace kstd {
namespace Memory {

void *
copy(void *to,
     const void *from,
     size_t length)
{ }


void *
move(void *to,
     const void *from,
     size_t length)
{ }


void *
zero(void *s,
     size_t length)
{ }


void *
set(void *s,
    uint8_t value,
    size_t length)
{ }
    
} /* namespace Memory */
} /* namespace kstd */
