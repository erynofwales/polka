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
#include "kstd/PrintFormat.hh"

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

    kstd::printFormat("Loading Polka...\n");

    kstd::printFormat("Kernel image: start = 0x%08lX, end = 0x%08lX, size = %ld bytes\n",
                      startupInformation.kernelStart, startupInformation.kernelEnd,
                      startupInformation.kernelSize());

    auto multiboot = startupInformation.multibootInformation;
    kstd::printFormat("Multiboot: start = 0x%08lX\n", u32(multiboot));
    kstd::printFormat("Command line: \"%s\"\n", multiboot->commandLine());

    mMemoryManager.initialize(startupInformation);
}


void
Kernel::panic(const char* msg,
              ...)
{
    mConsole.clear(Console::Color::Magenta);

    kstd::printFormat("PANIC! PANIC! PANIC! :-(\n");

    va_list args;
    va_start(args, msg);
    kstd::printFormat(msg, args);
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
