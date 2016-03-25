/* Descriptors.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Oh god oh god. Descriptor tables.
 */

#include "Descriptors.hh"
#include "Console.hh"

namespace {

/**
 * Six byte field containing the length and a linear address where a descriptor
 * table lives. One of these is passed to the `lgdt` and `lidt` instructions.
 */
struct PseudoDescriptor
{
    uint16_t limit;
    uint32_t base;
} __attribute((__packed__));

}

namespace x86 {

/*
 * Public
 */

GDT::DescriptorSpec
GDT::DescriptorSpec::kernelSegment(uint32_t base,
                                   uint32_t limit,
                                   GDT::Type type)
{
    return {base, limit, true, true, false, false, true, DPL::Ring0, true, type};
}


Descriptor
GDT::DescriptorSpec::descriptor()
    const
{
    Descriptor descriptor = 0;

    const auto dpl = uint8_t(privilegeLevel) & 0x3;
    const auto typ = uint8_t(type) & 0xF;

    descriptor  = base         & 0xFF000000;   // Bits 31:24 of the base address.
    if (hasCoarseGranularity) {
        descriptor |= (1 << 23);               // Granularity field
    }
    if (has32BitOperations) {
        descriptor |= (1 << 22);               // D/B field
    }
    if (hasNative64BitCode) {
        descriptor |= (1 << 21);               // L field
    }
    if (available) {
        descriptor |= (1 << 20);               // AVL field
    }
    descriptor |= limit        & 0x000F0000;   // Bits 19:16 of the segment limit.
    if (isPresent) {
        descriptor |= (1 << 15);               // P field
    }
    descriptor |= (dpl << 13);                 // DPL field
    if (isCodeDataSegment) {
        descriptor |= (1 << 12);               // S field
    }
    descriptor |= (typ << 8);                  // Type field: see Type
    descriptor |= (base >> 16) & 0x000000FF;   // Bits 23:16 of the base address.

    // Shift everything up by 32 to make room for the lower 4 bytes
    descriptor <<= 32;

    descriptor |= base << 16;                  // Bits 15:00 of the base address.
    descriptor |= limit        & 0x0000FFFF;   // Bits 15:00 of the segment limit.

    return descriptor;
}


GDT::GDT()
    : mTable{0}
{ }


void
GDT::setDescriptor(size_t index,
                   const GDT::DescriptorSpec& spec)
{
    if (index >= GDT::Size) {
        return;
    }
    mTable[index] = spec.descriptor();
}


void
GDT::setNullDescriptor(size_t index)
{
    if (index >= GDT::Size) {
        return;
    }
    mTable[index] = 0;
}


void
GDT::load()
    const
{
    PseudoDescriptor gdt {Size * sizeof(Descriptor) - 1, uint32_t(&mTable)};
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

/*
 * Static
 */

IDT&
IDT::systemIDT()
{
    static IDT sIDT;
    return sIDT;
}


IDT::DescriptorSpec
IDT::DescriptorSpec::exceptionHandler(uint16_t segment,
                                      void (*handler)())
{
    return {segment, uint32_t(handler), true, DPL::Ring0, true, Type::Interrupt};
}

/*
 * Public
 */

Descriptor
IDT::DescriptorSpec::descriptor()
    const
{
    Descriptor descriptor = 0;

    if (type != Type::Task) {
        descriptor = offset & 0xFFFF0000;
    }

    const auto dpl = uint8_t(privilegeLevel) & 0x3;
    const auto typ = uint8_t(type) & 0x7;

    if (isPresent) {
        descriptor |= 1 << 15;
    }
    descriptor |= dpl << 13;
    if (type != Type::Task && is32BitGate) {
        descriptor |= 1 << 11;
    }
    descriptor |= typ << 8;

    // Shift everything up by 32 to make room for the lower 4 bytes
    descriptor <<= 32;

    descriptor |= segment << 16;

    if (type != Type::Task) {
        descriptor |= offset & 0x0000FFFF;
    }

    return descriptor;
}


IDT::IDT()
    : mTable{0}
{ }


void
IDT::setDescriptor(size_t index,
                   const DescriptorSpec& spec)
{
    if (index >= Size) {
        return;
    }
    mTable[index] = spec.descriptor();
}


void
IDT::setNullDescriptor(size_t index)
{
    if (index >= Size) {
        return;
    }
    mTable[index] = 0;
}


void
IDT::load()
    const
{
    PseudoDescriptor idt{Size * sizeof(Descriptor) - 1, uint32_t(&mTable)};
    asm volatile("lidt %0\n" : : "m" (idt));
}

} /* namespace x86 */
