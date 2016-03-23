/* Memory.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Basic, low-level memory utilities.
 */

#include "Memory.hh"

namespace kstd {
namespace Memory {

void*
copy(void* to,
     const void* from,
     usize length)
{
    if (to == from) {
        return to;
    }

    auto toBytes = reinterpret_cast<u8*>(to);
    auto fromBytes = reinterpret_cast<const u8*>(from);
    auto toBytesEnd = toBytes + length;
    auto fromBytesEnd = fromBytes + length;

    if ((toBytes <= fromBytes && toBytesEnd > fromBytes) || (fromBytes <= toBytes && fromBytesEnd > toBytes)) {
        // Memory regions cannot overlap. Do nothing...silently. >_<
        return to;
    }

    // TODO: Copy.

    return to;
}


void*
move(void* to,
     const void *from,
     usize length)
{
    if (to == from) {
        return to;
    }

    auto toBytes = reinterpret_cast<u8*>(to);
    auto fromBytes = reinterpret_cast<const u8*>(from);
    auto toBytesEnd = toBytes + length;
    auto fromBytesEnd = fromBytes + length;

    if ((toBytes <= fromBytes && toBytesEnd > fromBytes) || (fromBytes <= toBytes && fromBytesEnd > toBytes)) {
        // We have to work backwards here to avoid copying over stuff that we still need to copy.
    } else {
        // Straight forward move.
    }

    return to;
}


void*
zero(void* ptr,
     usize length)
{
    auto p = reinterpret_cast<u8*>(ptr);

    while (length > 8) {
        *reinterpret_cast<u64*>(p) = u64(0);
        p += 8;
        length -= 8;
    }
    while (length > 4) {
        *reinterpret_cast<u32*>(p) = u32(0);
        p += 4;
        length -= 4;
    }
    while (length > 2) {
        *reinterpret_cast<u16*>(p) = u16(0);
        p += 2;
        length -= 2;
    }
    while (length > 0) {
        *p++ = u8(0);
        length--;
    }

    return ptr;
}


void*
set(void* ptr,
    u8 value,
    usize length)
{
    auto bytes = reinterpret_cast<u8*>(ptr);
    for (usize i = 0; i < length; i++) {
        bytes[i] = value;
    }
    return ptr;
}

} /* namespace Memory */
} /* namespace kstd */
