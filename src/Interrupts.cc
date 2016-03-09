/* Interrupts.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Implementation of interrupt handers and things.
 */

#include "Interrupts.hh"

namespace x86 {

/*
 * Static
 */

InterruptHandler&
InterruptHandler::systemInterruptHandler()
{
    static InterruptHandler sInterruptHandler;
    return sInterruptHandler;
}

/*
 * Public
 */

InterruptHandler::InterruptHandler()
    : mPIC(),
      mIDT()
{ }


void
InterruptHandler::initialize()
{
    mIDT.load();
    mPIC.remap(0x20, 0x28, 2);  // Map hardware IRQs to interrupt vectors 32 through 48.
}


void
InterruptHandler::enableInterrupts()
    const
{
    asm("sti");
}


void
InterruptHandler::disableInterrupts()
    const
{
    asm("cli");
}

} /* namespace x86 */
