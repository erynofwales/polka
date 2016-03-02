/* IO.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Low-level hardware I/O functions.
 */

#ifndef __IO_HH__
#define __IO_HH__

#include <stdint.h>

namespace kernel {
namespace io {
    
/*
 * Input
 */

inline uint8_t
inb(uint16_t port)
{
    uint8_t ret;
    asm volatile("inb %[port], %[ret]",
                 : [result] "=a"(ret)
                 : [port] "Nd"(port));
    return ret;
}


inline uint16_t
inw(uint16_t port)
{
    uint16_t ret;
    asm volatile("inw %[port], %[ret]",
                 : [result] "=a"(ret)
                 : [port] "Nd"(port));
    return ret;
}


inline uint32_t
inl(uint16_t port)
{
    uint32_t ret;
    asm volatile("inl %[port], %[ret]",
                 : [result] "=a"(ret)
                 : [port] "Nd"(port));
    return ret;
}

/*
 * Output
 */

inline void
outb(uint16_t port,
     uint8_t value)
{
    asm volatile("outb %0, %1" : : "a"(value), "Nd"(port));
}


inline void
outw(uint16_t port,
     uint16_t value)
{
    asm volatile("outw %0, %1" : : "a"(value), "Nd"(port));
}


inline void
outl(uint16_t port,
     uint32_t value)
{
    asm volatile("outl %0, %1" : : "a"(value), "Nd"(port));
}

} /* namespace io */
} /* namespace kernel */

#endif /* __IO_HH__ */
