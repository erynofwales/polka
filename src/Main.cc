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


/** The beginning of the world... */
extern "C"
void
kmain(multiboot::Information *information,
      u32 magic)
{
    multiboot::Information::setInformation(information);
    auto info = multiboot::Information::information();

    auto& kernel = kernel::Kernel::systemKernel();
    kernel.initialize();

    auto& console = kernel.console();

    console.printFormat("Command line: \"%s\"\n", info->commandLine());

    console.printFormat("Memory map:\n");
    console.printFormat("  available: lower = %ld KB, upper = %ld KB\n", info->lowerMemoryKB(), info->upperMemoryKB());
    for (auto it = info->memoryMapBegin(); it != info->memoryMapEnd(); ++it) {
        auto begin = (*it).base;
        auto end = begin + (*it).length - 1;
        console.printFormat("  begin = 0x%08lX %08lX, end = 0x%08lX %08lX (%s)\n",
                            u32(begin >> 32), u32(begin & 0xFFFFFFFF),
                            u32(end >> 32), u32(end & 0xFFFFFFFF),
                            (*it).type == 1 ? "available" : "reserved");
    }

    auto& gdt = x86::GDT::systemGDT();
    gdt.setNullDescriptor(0);
    gdt.setDescriptor(1, x86::GDT::DescriptorSpec::kernelSegment(0, 0xFFFFFFFF, x86::GDT::Type::CodeEXR));
    gdt.setDescriptor(2, x86::GDT::DescriptorSpec::kernelSegment(0, 0xFFFFFFFF, x86::GDT::Type::DataRW));
    gdt.load();
    console.printString("GDT loaded\n");

    auto& interruptHandler = x86::InterruptHandler::systemInterruptHandler();
    interruptHandler.initialize();
    interruptHandler.enableInterrupts();
    console.printString("Interrupts enabled\n");

    kernel::Kernel::systemKernel().halt();
}
