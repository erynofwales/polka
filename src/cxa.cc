/* cxa.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Implements some functions required by the C++ ABI so that local static
 * variables can be initialized properly.
 */

#include "Console.hh"

namespace __cxxabiv1 {

// 64-bit integer type to service as mutex-like guard.
__extension__ typedef int __guard __attribute__((mode(__DI__)));

extern "C" {

/*
 * TODO: The functions below are required by the C++ ABI and should be thread
 * safe. Since Polka currently has no threads, it should be fine to do this
 * without any actual locking. Once/If I add threading, I need to come back here
 * and add an actual mutex with a test-and-set.
 *
 * See http://wiki.osdev.org/C%2B%2B#Local_Static_Variables_.28GCC_Only.29
 * See the Intel XCHG instruction
 */

int
__cxa_guard_acquire(__guard *g)
{
    return !*(char *)(g);
}


void
__cxa_guard_release(__guard *g)
{
    *(char *)g = 1;
}


void
__cxa_guard_abort(__guard *)
{
    kernel::Console console;
    console.clear(kernel::Console::Color::Red);
    console.printString("OOPS!\n");
    console.printString("Unhandled exception initializing static variable.\n");
}

}

} /* namespace __cxxabiv1 */
