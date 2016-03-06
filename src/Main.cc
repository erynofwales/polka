#include <stddef.h>
#include <stdint.h>
#include "Console.hh"
#include "Descriptors.hh"
#include "PIC.hh"

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

    auto gdt = kernel::GDT::systemGDT();
    gdt.setNullDescriptor(0);
    gdt.setDescriptor(1, kernel::GDT::DescriptorSpec::kernelSegment(0, 0x000FFFFF, kernel::GDT::Type::CodeEXR));
    gdt.setDescriptor(2, kernel::GDT::DescriptorSpec::kernelSegment(0, 0x000FFFFF, kernel::GDT::Type::DataRW));
    gdt.load();

    console.writeString("GDT loaded\n");

    auto idt = kernel::IDT::systemIDT();
    idt.load();

    console.writeString("IDT loaded\n");

    auto pic = kernel::x86::PIC::systemPIC();
    pic.remap(0x20, 0x28, 2);   // Map hardware IRQs to interrupt vectors 32 through 48.

    console.writeString("Hardware interrupts programmed\n");
}
