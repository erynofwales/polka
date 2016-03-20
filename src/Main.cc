/* Main.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Entry point for the kernel in C/C++.
 */

#include <stddef.h>
#include <stdint.h>
#include "Console.hh"
#include "Descriptors.hh"
#include "Interrupts.hh"
#include "Multiboot.hh"

#if defined(__linux__)
#error "This file should be compiled with a cross-compiler, not the Linux system compiler!"
#endif

#if !defined(__i386__)
#error "This file should be compiled with an ix86-elf compiler!"
#endif

/** Called very early, before global initialization. */
extern "C"
void
kearly()
{
    using kernel::Console;

    // Create a console object for early use because global initialization hasn't happened yet.
    Console console;
    console.clear(kernel::Console::Color::Blue);
    console.printString("Loading system ...\n");
}


/** The beginning of the world... */
extern "C"
void
kmain(multiboot::Information *information)
{
    multiboot::Information::setInformation(information);
    auto info = multiboot::Information::information();

    // Reinitialize the system console now that we have global static objects.
    auto& console = kernel::Console::systemConsole();
    console.clear(kernel::Console::Color::Blue);

    console.printString("Loading Polka ...\n");

    console.printFormat("Command line: \"%s\"\n", info->commandLine());

    console.printFormat("Memory map:\n");
    console.printFormat("  available: lower = %ld KB, upper = %ld KB\n", info->lowerMemoryKB(), info->upperMemoryKB());
    // TODO: Print memory map, after determining that the info struct is correct.

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

    for (;;) { }
}
