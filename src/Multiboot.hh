/* Multiboot.hh
 * vim: set tw=120:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Definition of the Multiboot structure. When grub boots, it fills in this
 * structure and leaves a pointer to it in the EBX register.
 */

#ifndef __MULTIBOOT_HH__
#define __MULTIBOOT_HH__

#include "stdint.h"
#include "Attributes.hh"
#include "kstd/Types.hh"


namespace multiboot {

struct PACKED MemoryChunk
{
    /** Base address of the chunk of memory. */
    u64 base;
    /** Length of the chunk of memory, in bytes. */
    u64 length;
    /** Type of address range. 1 indicates available; all other values indicate reserved memory. */
    u32 type;
};

/**
 * The multiboot information struct. Defined by the multiboot spec.
 * See http://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Machine-state
 */
struct PACKED Information
{
    struct MemoryMapIterator
    {
        MemoryMapIterator(u32 address, u32 count);
        MemoryMapIterator(const MemoryMapIterator& other);

        MemoryMapIterator& operator++();
        MemoryMapIterator operator++(int);
        MemoryChunk operator*();

        bool operator==(const MemoryMapIterator& other) const;
        bool operator!=(const MemoryMapIterator& other) const;

    private:
        u32 mCurrent;
        u32 mLength;
    };

    /** Size of lower memory in KB. */
    u32 lowerMemoryKB() const;
    /** Size of upper memory in KB. */
    u32 upperMemoryKB() const;
    /** Total size of memory in KB. */
    u32 memoryKB() const;

    const char* commandLine() const;

    MemoryMapIterator memoryMapBegin() const;
    MemoryMapIterator memoryMapEnd() const;

private:
    /** Bit field of flags. Fields below are only defined if the appropriate flag is set. */
    u32 mFlags;

    /** Amount of lower (0 to 1 MB) memory, in KB. */
    u32 mMemLower;
    /** Amount of upper (1 MB to ...) memory, in KB. Maximally, this value is the address of the first upper memory hole, minus 1 MB. */
    u32 mMemUpper;

    /** Indicates which BIOS disk the boot loader loaded the OS image from. */
    struct PACKED {
        /** Third level partition number. If unused this is set to 0xFF. */
        u8 partitionLevel3;
        /** Sub-partition number. If unused this is set to 0xFF. */
        u8 partitionLevel2;
        /** Top-level partition number. */
        u8 partitionLevel1;
        /** BIOS drive number, as returned by the `INT 0x13` low-level disk interface. */
        u8 driveNumber;
    } mBootDevice;

    /** Pointer to a C-style string containing the command line arguments. */
    u32 mCommandLine;

    /**
     * Multiboot Modules
     * Indicates what boot modules were loaded along with the kernel image.
     * Defined only if `modulesPresent == true`.
     * @{
     */
    /** Number of boot modules present. */
    u32 mModulesCount;
    /** Pointer to start of boot modules array. */
    u32 mModulesAddress;
    /** @} */

    // TODO: Document these.
    union PACKED {
        struct PACKED {
            u32 tableSize;
            u32 stringSize;
            u32 address;
            u32 reserved;
        } aout;
        struct PACKED {
            u32 number;
            u32 size;
            u32 address;
            u32 shndx; // TODO: Bad name. What is this?
        } elf;
    } symbols;

    /**
     * @defgroup Memory Map
     * Defines a buffer containing a memory map provided by the BIOS.
     * @{
     */
    /** Length of the buffer, in bytes. The spec is somewhat unclear on this field. */
    u32 mMemoryMapLength;
    /** Pointer to start of memory map entry array. */
    u32 mMemoryMapAddress;
    /** @} */

    /**
     * @defgroup Drives
     * Defines a buffer containing a list of drive definitions provided by the BIOS.
     * @{
     */
    /** Number of memory map entries present. */
    u32 mDrivesLength;
    /** Pointer to start of memory map entry array. */
    u32 mDrivesAddress;
    /** @} */

    /** Pointer to a table containing APM information. */
    u32 mAPMTable;

    /** I dunno some VBE stuff. TODO. */
    struct PACKED {
        u32 controlInformation;
        u32 modeInformation;
        u32 mode;
        u32 interfaceSegment;
        u32 interfaceOff;
        u32 interfaceLength;
    } vbe;
};

struct PACKED Module
{
    /** Start address of boot module. */
    u32 start;
    /** End address of boot module. */
    u32 end;
    /** Pointer to a C-style boot module string. */
    u32 string;
    /** Reserved and ignored. */
    u32 reserved;
};

// TODO: Define the Drive struct
struct PACKED Drive
{
};

} /* namespace multiboot */

#endif /* __MULTIBOOT_HH__ */
