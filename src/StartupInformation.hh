/* StartupInformation.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Declaration of the StartupInformation struct.
 */

#ifndef __STARTUPINFORMATION_HH__
#define __STARTUPINFORMATION_HH__

#include "Multiboot.hh"
#include "kstd/Types.hh"


namespace kernel {

/** Collection of useful tidbits for setting up the system. */
struct StartupInformation
{
    /** Starting address of the kernel. 4K aligned. */
    u32 kernelStart;
    /** Ending address (the first valid address *after* the last) of the kernel. 4K aligned. */
    u32 kernelEnd;

    /** Multiboot's magic value. This should be verified. */
    u32 multibootMagic;
    /** Pointer to the multiboot information struct. */
    multiboot::Information* multibootInformation;

    /** Size of the kernel image in bytes. */
    u32 kernelSize() const;

    /** Size of memory in bytes. */
    u32 memorySize() const;
};

} /* namespace kernel */

#endif /* __STARTUPINFORMATION_HH__ */
