/* Kernel.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Kernel object. This is the highest level object in the system.
 */

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
    : mConsole()
{ }


void
Kernel::initialize()
{
    mConsole.clear(kernel::Console::Color::Blue);
    mConsole.printString("Loading Polka...\n");
}


void
Kernel::panic(const char* msg,
              ...)
{
    mConsole.clear(Console::Color::Magenta);
    mConsole.printString("PANIC! PANIC! PANIC! :-(\n");
    mConsole.printString(msg);
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
