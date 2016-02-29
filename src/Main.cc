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
    auto console = kernel::Console::systemConsole();
    console.clear(kernel::Console::Color::Blue);
    console.writeString("Loading system ...\n");

    kernel::initGDT();

    volatile int foo = 0;
    int i = 0;
    for (;;) {
        if (i == 0) {
            console.writeString("--- MARK ---\n");
        }
        console.writeChar('a' + i);
        console.writeChar('\n');
        i = (i + 1) % 26;

        for (uint32_t k = 0; k < (2u << 20) - 1; k++) {
            foo /= 2;
        }
    }
}


/** The beginning of the world... */
extern "C"
void
kmain()
{ }
