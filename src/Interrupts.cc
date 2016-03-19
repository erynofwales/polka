/* Interrupts.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Implementation of interrupt handers and things.
 */

#include "Interrupts.hh"
#include "Console.hh"
#include "IO.hh"

extern "C" {
    // Assembly functions. See isr.S.
    void unhandledInterrupt();
    void handleDEException();
    void handleNMIInterrupt();
    void handleDFException();
    void handleGPException();
    void handleHardwareInterrupt0();
    void handleHardwareInterrupt1();
}

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
    auto& console = kernel::Console::systemConsole();

    // All interrupts start out as "unhandled"
    for (size_t i = 0; i < IDT::Size; i++) {
        mIDT.setDescriptor(i, IDT::DescriptorSpec::exceptionHandler(0x8, &unhandledInterrupt));
    }

    // System exceptions/faults
    mIDT.setDescriptor(0x00, IDT::DescriptorSpec::exceptionHandler(0x8, &handleDEException));
    mIDT.setDescriptor(0x02, IDT::DescriptorSpec::exceptionHandler(0x8, &handleNMIInterrupt));
    mIDT.setDescriptor(0x08, IDT::DescriptorSpec::exceptionHandler(0x8, &handleDFException));
    mIDT.setDescriptor(0x0D, IDT::DescriptorSpec::exceptionHandler(0x8, &handleGPException));


    // Hardware interrupts
    mIDT.setDescriptor(0x20, IDT::DescriptorSpec::exceptionHandler(0x8, &handleHardwareInterrupt0));
    mIDT.setDescriptor(0x21, IDT::DescriptorSpec::exceptionHandler(0x8, &handleHardwareInterrupt1));

    mIDT.load();

    mPIC.initialize(0x20, 0x28);  // Map hardware IRQs to interrupt vectors 32 through 48.
    console.printString("Hardware interrupts initialized\n");

    // Enable the keyboard interrupt.
    mPIC.enableInterrupt(1, true);
    console.printString("Enabling keyboard interrupt\n");
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


void
InterruptHandler::dispatchException(uint8_t exception)
{
    using x86::Interrupt;

    auto& console = kernel::Console::systemConsole();
    console.printString("Received exception ");
    switch (Interrupt(exception)) {
        case Interrupt::DE:
            console.printString("#DE");
            break;
        case Interrupt::NMI:
            console.printString("NMI");
            break;
        case Interrupt::DF:
            console.printString("#DF");
            break;
        case Interrupt::GP:
            console.printString("#GP");
            break;
        default:
            console.printString("SOME OTHER THING");
            break;
    }
    console.printString("\nAbort. :(");
}


void
InterruptHandler::dispatchHardwareInterrupt(uint8_t irq)
{
    switch (irq) {
        case 0:
            doTimerInterrupt();
            break;
        case 1:
            doKeyboardInterrupt();
            break;
        default:
            // TODO: Implement a panic function...
            break;
    }
    finishHardwareInterrupt(irq);
}

/*
 * Private
 */

void
InterruptHandler::doTimerInterrupt()
{
    auto& console = kernel::Console::systemConsole();
    console.printString("Thyme!\n");
}


void
InterruptHandler::doKeyboardInterrupt()
{
    // Quick 'n dirty read the scancode.
    unsigned int c = 0;
    auto& console = kernel::Console::systemConsole();
    do {
        c = kernel::io::inw(0x60);
        console.printFormat("Key! (scancode 0x%02X)\n", c);
    } while (c == 0);
}


inline void
InterruptHandler::finishHardwareInterrupt(uint8_t irq)
    const
{
    mPIC.endOfInterrupt(irq);
}

} /* namespace x86 */

/*
 * Interrupt handlers
 */

extern "C"
void
doUnhandledInterrupt()
{
    auto& console = kernel::Console::systemConsole();
    console.printString("Received unhandled interrupt.\nAbort. :(");
}


extern "C"
void
dispatchExceptionHandler(size_t vector)
{
    x86::InterruptHandler::systemInterruptHandler().dispatchException(vector);
}


extern "C"
void
dispatchHardwareInterrupt(size_t irq)
{
    x86::InterruptHandler::systemInterruptHandler().dispatchHardwareInterrupt(irq);
}
