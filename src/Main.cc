/* Main.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Entry point for the kernel in C/C++.
 */

#include <stddef.h>
#include "Console.hh"
#include "Descriptors.hh"
#include "Interrupts.hh"
#include "Kernel.hh"
#include "Multiboot.hh"
#include "kstd/Types.hh"

#if defined(__linux__)
#error "This file should be compiled with a cross-compiler, not the Linux system compiler!"
#endif

#if !defined(__i386__)
#error "This file should be compiled with an ix86-elf compiler!"
#endif

// Linker script defined symbols. See linker.ld.
extern u32 kernelStart;
extern u32 kernelEnd;


/** The beginning of the world... */
extern "C"
void
kmain(multiboot::Information *information,
      u32 magic)
{
    auto& kernel = kernel::Kernel::systemKernel();

    kernel::StartupInformation startupInformation;
    startupInformation.kernelStart = u32(&kernelStart);
    startupInformation.kernelEnd = u32(&kernelEnd);
    startupInformation.multibootInformation = information;

    kernel.initialize(startupInformation);

    auto& console = kernel.console();

    auto& interruptHandler = x86::InterruptHandler::systemInterruptHandler();
    interruptHandler.initialize();
    interruptHandler.enableInterrupts();
    console.printString("Interrupts enabled\n");

    kernel::Kernel::systemKernel().halt();
}
