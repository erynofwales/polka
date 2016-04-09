/* Kernel.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Kernel object. This is the highest level object in the system.
 */

#ifndef __KERNEL_HH__
#define __KERNEL_HH__

#include "Attributes.hh"
#include "Console.hh"
#include "Multiboot.hh"
#include "StartupInformation.hh"
#include "kstd/Types.hh"
#include "memory/Memory.hh"


namespace kernel {

/** The kernel itself. */
struct Kernel
{
    static Kernel& systemKernel();

    Kernel();

    /** Initialize the system. */
    void initialize(const StartupInformation& startupInformation);

    /**
     * Put up a panic screen and halt the system.
     * @see halt()
     */
    void panic(const char* msg, ...);

    /** Disable interrupts and halt the system. You will never return from that place... */
    void halt() NORETURN;

    Console& console();

private:
    Console mConsole;
    MemoryManager mMemoryManager;
};

} /* namespace kernel */

#endif /* __KERNEL_HH__ */
