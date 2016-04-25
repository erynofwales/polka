/* Memory.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Declaration of top-level classes for managing system memory.
 */

#ifndef __MEMORY_MEMORY_HH__
#define __MEMORY_MEMORY_HH__

#include "Console.hh"
#include "Descriptors.hh"
#include "StartupInformation.hh"
#include "memory/FrameAllocator.hh"
#include "memory/PageAllocator.hh"


namespace kernel {

struct MemoryManager
{
    MemoryManager();

    void initialize(const StartupInformation& startupInformation);

private:
    x86::GDT mGDT;
    FrameAllocator mFrameAllocator;
    PageAllocator mPageAllocator;

    void initializeGDT();
};

namespace memory {

/** Size of a single page in bytes. */
extern const usize pageSize;
extern const usize pageMask;


/** Align to the nearest page boundary below `addr`. */
inline uptr
pageAlignDown(uptr addr)
{
    return addr & ~pageMask;
}


/** Align to the nearest page boundary above `addr`. */
inline uptr
pageAlignUp(uptr addr)
{
    if (pageAlignDown(addr) == addr) {
        return addr;
    }
    return (addr + pageSize) & ~pageMask;
}

} /* namespace memory */

} /* namespace kernel */

#endif /* __MEMORY_MEMORY_HH__ */
