/* Descriptors.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Declaration of relevant classes and functions for dealing with the GDT and
 * IDT.
 */

#ifndef __DESCRIPTORS_HH__
#define __DESCRIPTORS_HH__

#include <stddef.h>
#include <stdint.h>


namespace x86 {

/**
 * SegmentDescriptors are entries in the GDT and LDT that describe memory
 * segments. Each descriptor is two double-words (8 bytes, 64 bits) long.
 */
typedef uint64_t Descriptor;


/** Descriptor privilege level. */
enum class DPL : uint8_t {
    Ring0 = 0x0,
    Ring1 = 0x1,
    Ring2 = 0x2,
    Ring3 = 0x3
};


/**
 * Global Descriptor Table
 *
 * This class wraps the x86 Global Descriptor Table (GDT), which describes for
 * the CPU the memory segments present on the system.
 */
struct GDT
{
    /** A four bit value describing the type of the segment. */
    enum class Type : uint8_t {
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

    /**
     * Describes a memory segment for the GDT. See the Intel System Programming
     * Guide, page 3-10, for details.
     */
    struct DescriptorSpec
    {
        uint32_t base;              // Base address of segment
        uint32_t limit;             // Extent/length/size/limit of segment; 24 bits used
        bool hasCoarseGranularity;  // G field; coarse = limit in 4KByte units
        bool has32BitOperations;    // D/B field
        bool hasNative64BitCode;    // L field; only valid in IA-32e mode
        bool available;             // AVL field; available for system software use
        bool isPresent;             // P field
        DPL privilegeLevel;         // DPL field
        bool isCodeDataSegment;     // S field
        Type type;                  // Type field

        static DescriptorSpec kernelSegment(uint32_t base, uint32_t limit, Type type);

        Descriptor descriptor() const;
    };

    GDT();

    /** Set the descriptor at the given `index` to the value of `spec`. */
    void setDescriptor(size_t index, const DescriptorSpec& spec);

    /** Set the descriptor at the given `index` to the NULL descriptor. */
    void setNullDescriptor(size_t index);

    /** Load this GDT into the CPU and flush the registers. */
    void load() const;

private:
    // TODO: Maybe eventually I can make this variable? Maybe use templates?
    static const size_t Size = 5;

    Descriptor mTable[Size];
};


/**
 * Interrupt Descriptor Table
 *
 * This class wraps the x86 Interrupt Descriptor Table (IDT), which tells the
 * CPU how to handle interrupts. The table is a collection of gate descriptors,
 * each of which is responsible for responding to a particular exception or
 * interrupt.
 */
struct IDT
{
    enum class Type : uint8_t {
        Task = 0x5,
        Interrupt = 0x6,
        Trap = 0x7,
    };

    struct DescriptorSpec
    {
        static DescriptorSpec exceptionHandler(uint16_t segment, void (*handler)());

        uint16_t segment;
        uint32_t offset;
        bool isPresent;
        DPL privilegeLevel;
        bool is32BitGate;
        Type type;

        Descriptor descriptor() const;
    };

    /**
     * Size of the table. IDTs shouldn't have any more than this many
     * descriptors.
     */
    static const size_t Size = 256;

    static IDT& systemIDT();

    IDT();

    /** Set the descriptor at the given `index` to the value of `spec`. */
    void setDescriptor(size_t index, const DescriptorSpec& spec);

    /** Set the descriptor at the given `index` to the NULL descriptor. */
    void setNullDescriptor(size_t index);

    void load() const;

private:
    Descriptor mTable[Size];
};

} /* namespace x86 */

#endif /* __DESCRIPTORS_HH__ */
