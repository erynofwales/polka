/* Descriptors.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Oh god oh god. Descriptor tables.
 */

#include <stddef.h>
#include <stdint.h>
#include "Descriptors.hh"

namespace kernel {

/**
 * SegmentDescriptors are entries in the GDT and LDT that describe memory
 * segments. Each descriptor is two double-words (8 bytes, 64 bits) long.
 */
typedef uint64_t SegmentDescriptor;


/** Descriptor privilege level. */
enum class DPL {
    Ring0 = 0x0,
    Ring1 = 0x1,
    Ring2 = 0x2,
    Ring3 = 0x3
};


/** A four bit value describing the type of the segment. */
enum class Type {
    // Data segment types
    DataRO      = 0x0, // Read-only
    DataROA     = 0x1, // Read-only, accessed
    DataRW      = 0x2, // Read/write
    DataRWA     = 0x3, // Read/write, accessed
    DataROEX    = 0x4, // Read-only, expand-down
    DataROEXA   = 0x5, // Read-only, expand-down, accessed
    DataRWEX    = 0x6, // Read/write, expand-down
    DataRWEXA   = 0x7, // Read/write, expand-down, accessed

    // Code segment types
    CodeEX      = 0x8, // Execute-only
    CodeEXA     = 0x9, // Execute-only, accessed
    CodeEXR     = 0xa, // Execute/read
    CodeEXRA    = 0xb, // Execute/read, accessed
    CodeEXC     = 0xc, // Execute-only, conforming
    CodeEXCA    = 0xd, // Execute-only, conforming, accessed
    CodeEXRC    = 0xe, // Execute/read, conforming
    CodeEXRCA   = 0xf  // Execute/read, conforming, accessed
};


/** Six byte field containing the length and a linear address where the GDT lives. */
struct GDTPointer
{
    uint16_t limit;
    uint32_t base;
} __attribute((__packed__));


static const size_t GDTSize = 5;
static SegmentDescriptor sGDT[GDTSize];


static inline SegmentDescriptor
createSegmentDescriptor(uint32_t base,
                        uint32_t limit,
                        Type type,
                        DPL dpl)
{
    SegmentDescriptor descriptor = 0;

    uint8_t t = static_cast<uint8_t>(type);
    uint8_t d = static_cast<uint8_t>(dpl);

    descriptor  = base         & 0xFF000000;    // Bits 31:24 of the base address.
    descriptor |= (0x1 << 23);                  // Granularity field: segment limit is interpreted in 4KB units.
    descriptor |= (0x1 << 22);                  // D/B field: default operation/stack size flag, 1 for 32-bit.
    descriptor |= (0x0 << 21);                  // L field: 64-bit code segment, 0 for 32-bit.
    descriptor |= (0x0 << 20);                  // AVL field: system determined, unused here.
    descriptor |= limit        & 0x000F0000;    // Bits 19:16 of the segment limit.
    descriptor |= (0x1 << 15);                  // P field: segment is present.
    descriptor |= (d << 13)    & 0x00006000;    // DPL field: privilege level of the segment.
    descriptor |= (0x1 << 12);                  // S field: 0 for system, 1 for code/data.
    descriptor |= (t << 8)     & 0x00000F00;    // Type field: see Type
    descriptor |= (base >> 16) & 0x000000FF;    // Bits 23:16 of the base address.

    // Shift everything up by 32 to make room for the lower 4 bytes
    descriptor <<= 32;

    descriptor |= base << 16;                   // Bits 15:00 of the base address.
    descriptor |= limit        & 0x0000FFFF;    // Bits 15:00 of the segment limit.

    return descriptor;
}

/*
 * Static
 */

void
initGDT()
{
    sGDT[0] = 0;    // First descriptor is always NULL.
    sGDT[1] = createSegmentDescriptor(0x00000000, 0x000FFFFF, Type::CodeEXR, DPL::Ring0);
    sGDT[2] = createSegmentDescriptor(0x00000000, 0x000FFFFF, Type::DataRW, DPL::Ring0);
    sGDT[3] = 0;    // Unused for now.
    sGDT[4] = 0;    // Unused for now.

    GDTPointer gdt {GDTSize * sizeof(SegmentDescriptor) - 1, uint32_t(&sGDT)};

    /*
     * Load the new GDT with the pointer defined above. The GDT isn't actually
     * used until the segment registers are reladed. Below, CS is reloaded by
     * a long jump into the new code segment. The rest of the segment registers
     * can be loaded directly.
     */
    asm volatile(
        "lgdt %0\n"
        "ljmpl $0x08, $reloadSegments\n"
        "reloadSegments:\n"
        "movl $0x10, %%eax\n"
        "movl %%eax, %%ds\n"
        "movl %%eax, %%es\n"
        "movl %%eax, %%fs\n"
        "movl %%eax, %%gs\n"
        "movl %%eax, %%ss\n"
        : : "m" (gdt)
        : "%eax");
}

} /* namespace kernel */
