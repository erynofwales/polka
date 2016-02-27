#include <stddef.h>
#include <stdint.h>

#if defined(__linux__)
#error "This file should be compiled with a cross-compiler, not the Linux system compiler!"
#endif

#if !defined(__i386__)
#error "This file should be compiled with an ix86-elf compiler!"
#endif

/**
 * Called very early, before global initialization.
 */
extern "C"
void
kearly()
{ }


/**
 * The beginning of the world...
 */
extern "C"
void
kmain()
{ }
