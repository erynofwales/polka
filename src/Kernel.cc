/* Kernel.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Kernel object. This is the highest level object in the system.
 */

#include <stdarg.h>
#include "Kernel.hh"
#include "Interrupts.hh"

namespace {

static kernel::Kernel sKernel;

} /* anonymous namespace */

namespace kernel {

/*
 * Static
 */

Kernel&
Kernel::systemKernel()
{
    return sKernel;
}

/*
 * Public
 */

Kernel::Kernel()
    : mConsole(),
      mMemoryManager()
{ }


void
Kernel::initialize(const StartupInformation& startupInformation)
{
    mConsole.clear(kernel::Console::Color::Blue);
    mConsole.printString("Loading Polka...\n");

    mConsole.printFormat("Kernel image: start = 0x%08lX, end = 0x%08lX, size = %ld bytes\n", startupInformation.kernelStart, startupInformation.kernelEnd, startupInformation.kernelEnd - startupInformation.kernelStart);

    auto multiboot = startupInformation.multibootInformation;
    mConsole.printFormat("Multiboot: start = 0x%08lX\n", u32(multiboot));
    mConsole.printFormat("Command line: \"%s\"\n", multiboot->commandLine());

    mConsole.printFormat("Memory map:\n");
    mConsole.printFormat("  available: lower = %ld KB, upper = %ld KB\n", multiboot->lowerMemoryKB(), multiboot->upperMemoryKB());
    for (auto it = multiboot->memoryMapBegin(); it != multiboot->memoryMapEnd(); ++it) {
        auto begin = (*it).base;
        auto end = begin + (*it).length - 1;
        mConsole.printFormat("  begin = 0x%08lX %08lX, end = 0x%08lX %08lX (%s)\n",
                             u32(begin >> 32), u32(begin & 0xFFFFFFFF),
                             u32(end >> 32), u32(end & 0xFFFFFFFF),
                             (*it).type == 1 ? "available" : "reserved");
    }

    mMemoryManager.initialize(mConsole);
}


void
Kernel::panic(const char* msg,
              ...)
{
    mConsole.clear(Console::Color::Magenta);
    mConsole.printString("PANIC! PANIC! PANIC! :-(\n");

    va_list args;
    va_start(args, msg);
    mConsole.printFormat(msg, args);
    va_end(args);

    // TODO: Dump registers.

    halt();
}

Console&
Kernel::console()
{
    return mConsole;
}

/*
 * Private
 */

void
Kernel::halt()
{
    x86::InterruptHandler::systemInterruptHandler().disableInterrupts();
    for (;;) {
        asm("hlt");
    }
}

} /* namespace kernel */
