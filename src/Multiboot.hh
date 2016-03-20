/* Multiboot.hh
 * vim: set tw=80:
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

namespace multiboot {

struct PACKED MemoryChunk
{
    /** Base address of the chunk of memory. */
    uint64_t base;
    /** Length of the chunk of memory, in bytes. */
    uint64_t length;
    /** Type of address range. 1 indicates available; all other values indicate reserved memory. */
    uint32_t type;
};

/**
 * The multiboot information struct. Defined by the multiboot spec.
 * See http://www.gnu.org/software/grub/manual/multiboot/multiboot.html#Machine-state
 */
struct PACKED Information
{
    struct MemoryMapIterator
    {
        MemoryMapIterator(uint32_t address, uint32_t count);
        MemoryMapIterator(const MemoryMapIterator& other);

        MemoryMapIterator& operator++();
        MemoryMapIterator operator++(int);
        MemoryChunk operator*();

        bool operator==(const MemoryMapIterator& other) const;
        bool operator!=(const MemoryMapIterator& other) const;

    private:
        MemoryChunk *mCurrent;
        uint32_t mCount;
    };

    static const Information *information();
    static void setInformation(Information* info);

    uint32_t lowerMemoryKB() const;
    uint32_t upperMemoryKB() const;

    const char* commandLine() const;

    uint32_t memoryMapChunks() const;
    MemoryMapIterator memoryMapBegin() const;
    MemoryMapIterator memoryMapEnd() const;

private:
    /** Bit field of flags. Fields below are only defined if the appropriate flag is set. */
    uint32_t mFlags;

    /** Amount of lower (0 to 1 MB) memory, in KB. */
    uint32_t mMemLower;
    /** Amount of upper (1 MB to ...) memory, in KB. Maximally, this value is the address of the first upper memory hole, minus 1 MB. */
    uint32_t mMemUpper;

    /**
     * Indicates which BIOS disk the boot loader loaded the OS image from.
     * Defined only if `bootDevice == true`.
     */
    struct PACKED
    {
        /** Third level partition number. If unused this is set to 0xFF. */
        uint8_t partitionLevel3;
        /** Sub-partition number. If unused this is set to 0xFF. */
        uint8_t partitionLevel2;
        /** Top-level partition number. */
        uint8_t partitionLevel1;
        /**
         * BIOS drive number, as returned by the `INT 0x13` low-level disk
         * interface.
         */
        uint8_t driveNumber;
    } bootDevice;

    /**
     * Pointer to a C-style string containing the command line arguments.
     * Defined only if `commandLinePresent == true`.
     */
    uint32_t mCommandLine;

    /**
     * Indicates what boot modules were loaded along with the kernel image.
     * Defined only if `modulesPresent == true`.
     */
    struct PACKED
    {
        /** Number of boot modules present. */
        uint32_t count;
        /** Pointer to start of boot modules array. */
        uint32_t address;
    } modules;

    // TODO: Document these.
    union PACKED
    {
        struct PACKED
        {
            uint32_t tableSize;
            uint32_t stringSize;
            uint32_t address;
            uint32_t reserved;
        } aout;

        struct PACKED
        {
            uint32_t number;
            uint32_t size;
            uint32_t address;
            uint32_t shndx;
        } elf;
    } symbols;

    /**
     * @defgroup Memory Map
     * Points to a buffer containing a memory map of the machine provided by the
     * BIOS. Defined only if `memoryMapPresent == true`.
     * @{
     */
    /** Number of memory map entries present. */
    uint32_t memoryMapCount;
    /** Pointer to start of memory map entry array. */
    uint32_t memoryMapAddress;
    /** @} */

    /**
     * Points to a buffer containing a list of drive definitions provided by the
     * BIOS. Defined only if `drivesPresent == true`.
     */
    struct PACKED
    {
        /** Number of memory map entries present. */
        uint32_t count;
        /** Pointer to start of memory map entry array. */
        uint32_t address;
    } drives;

    /**
     * Pointer to the table containing APM information. Defined only if
     * `apmTablePresent == true`.
     */
    uint32_t apmTable;

    /**
     * I dunno some VBE stuff. TODO.
     */
    struct PACKED {
        uint32_t controlInformation;
        uint32_t modeInformation;
        uint32_t mode;
        uint32_t interfaceSegment;
        uint32_t interfaceOff;
        uint32_t interfaceLength;
    } vbe;
};

struct PACKED Module
{
    /** Start address of boot module. */
    uint32_t start;
    /** End address of boot module. */
    uint32_t end;
    /** Pointer to a C-style boot module string. */
    uint32_t string;
    /** Reserved and ignored. */
    uint32_t reserved;
};

// TODO: Define the Drive struct
struct PACKED Drive
{
};

} /* namespace multiboot */

#endif /* __MULTIBOOT_HH__ */
