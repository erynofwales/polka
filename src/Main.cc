#include <stddef.h>
#include <stdint.h>
#include "Console.hh"

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
{ }


/** The beginning of the world... */
extern "C"
void
kmain()
{
    auto console = kernel::Console::systemConsole();
    console.clear(kernel::Console::Color::Blue);
    console.writeString("Hello world!\n");

    // TODO: The performance of this loop slowed down a *lot* (7-8 orders of magnitude according to the busy loop) when I moved this code from kearly() to here. I wonder if it has something to do with object initialization? Should probably investigate some.
    volatile int foo = 0;
    int j = 0;
    int i = 0;
    for (;;) {
        if (j == 0) {
            console.writeString("--- MARK ---\n");
        }
        console.writeChar('a' + i);
        console.writeChar('\n');
        i = (i + 1) % 26;
        j = (j + 1) % 500;

        for (uint32_t k = 0; k < (2u << 20) - 1; k++) {
            foo /= 2;
        }
    }
}
