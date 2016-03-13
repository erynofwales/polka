/* Interrupts.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Implementation of interrupt handers and things.
 */

#include "Interrupts.hh"
#include "Console.hh"

extern "C" {
    void dispatchExceptionHandler(size_t vector);

    // Assembly functions. See isr.S.
    void unhandledInterrupt();
    void handleDEException();
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

    for (size_t i = 0; i < IDT::Size; i++) {
        mIDT.setDescriptor(i, IDT::DescriptorSpec::exceptionHandler(1, &unhandledInterrupt));
    }
    mIDT.load();
    console.writeString("Interrupt table loaded\n");

    mPIC.initialize(0x20, 0x28);  // Map hardware IRQs to interrupt vectors 32 through 48.
    console.writeString("Hardware interrupts initialized\n");

    // Enable the keyboard interrupt.
    mPIC.enableInterrupt(1, true);
    console.writeString("Enabling keyboard interrupt\n");
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


extern "C"
void
doUnhandledInterrupt()
{
    auto& console = kernel::Console::systemConsole();
    console.writeString("Received unhandled interrupt.\nAbort. :(");
}


extern "C"
void
dispatchExceptionHandler(size_t vector)
{
    using x86::Interrupt;

    auto& console = kernel::Console::systemConsole();
    console.writeString("Received exception ");
    switch (Interrupt(vector)) {
        case Interrupt::DE:
            console.writeString("#DE");
            break;
        case Interrupt::NMI:
            console.writeString("NMI");
            break;
        case Interrupt::DF:
            console.writeString("#DF");
            break;
        case Interrupt::GP:
            console.writeString("#GP");
            break;
        default:
            console.writeString("SOME OTHER THING");
            break;
    }
    console.writeString("\nAbort. :(");
}

extern "C"
void
handleTimerInterrupt()
{
    auto& console = kernel::Console::systemConsole();
    console.writeString("Thyme!\n");
}

extern "C"
void
handleKeyboardInterrupt()
{
    auto& console = kernel::Console::systemConsole();
    console.writeString("Key!\n");
}
