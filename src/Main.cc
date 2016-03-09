#include <stddef.h>
#include <stdint.h>
#include "Console.hh"
#include "Descriptors.hh"
#include "Interrupts.hh"

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

    /*
     * Create a console object for early use because global initialization
     * hasn't happened yet.
     */
    Console console;
    console.clear(kernel::Console::Color::Blue);
    console.writeString("Loading system ...\n");
}


/** The beginning of the world... */
extern "C"
void
kmain()
{
    // Reinitialize the system console now that we have global static objects.
    auto console = kernel::Console::systemConsole();
    console.clear(kernel::Console::Color::Blue);

    auto gdt = x86::GDT::systemGDT();
    gdt.setNullDescriptor(0);
    gdt.setDescriptor(1, x86::GDT::DescriptorSpec::kernelSegment(0, 0x000FFFFF, x86::GDT::Type::CodeEXR));
    gdt.setDescriptor(2, x86::GDT::DescriptorSpec::kernelSegment(0, 0x000FFFFF, x86::GDT::Type::DataRW));
    gdt.load();

    console.writeString("GDT loaded\n");

    auto interruptHandler = x86::InterruptHandler::systemInterruptHandler();
    interruptHandler.initialize();
}
