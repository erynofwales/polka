#include <stddef.h>
#include <stdint.h>
#include "Console.hh"
#include "Descriptors.hh"

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
    using kernel::Console;
    using kernel::GDT;
    using kernel::IDT;

    // Reinitialize the system console now that we have global static objects.
    auto console = Console::systemConsole();
    console.clear(Console::Color::Blue);

    auto gdt = GDT::systemGDT();
    gdt.setNullDescriptor(0);
    gdt.setDescriptor(1, GDT::DescriptorSpec::kernelSegment(0, 0x000FFFFF, GDT::Type::CodeEXR));
    gdt.setDescriptor(2, GDT::DescriptorSpec::kernelSegment(0, 0x000FFFFF, GDT::Type::DataRW));
    gdt.load();

    console.writeString("GDT loaded\n");

    auto idt = IDT::systemIDT();
    idt.load();

    console.writeString("IDT loaded\n");
}
