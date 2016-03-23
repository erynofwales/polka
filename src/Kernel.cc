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

void
Kernel::panic(const char* msg,
              ...)
{

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
