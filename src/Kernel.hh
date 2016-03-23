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


namespace kernel {

struct Kernel
{
    static Kernel& systemKernel();

    Kernel();

    void initialize();

    void panic(const char* msg, ...);

    void halt() NORETURN;

private:
    Console mConsole;
};

} /* namespace kernel */

#endif /* __KERNEL_HH__ */
