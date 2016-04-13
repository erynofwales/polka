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


namespace kernel {

struct MemoryManager
{
    MemoryManager();

    void initialize(const StartupInformation& startupInformation);

private:
    x86::GDT mGDT;
    kernel::FrameAllocator mFrameAllocator;

    void initializeGDT();
};

namespace memory {

extern const u32 pageSize;
extern const u32 pageMask;

/** Align to the nearest page boundary below `addr`. */
inline u32
pageAlignDown(u32 addr)
{
    return addr & ~pageMask;
}

/** Align to the nearest page boundary above `addr`. */
inline u32
pageAlignUp(u32 addr)
{
    if (pageAlignDown(addr) == addr) {
        return addr;
    }
    return (addr + pageSize) & ~pageMask;
}

} /* namespace memory */

} /* namespace kernel */

#endif /* __MEMORY_MEMORY_HH__ */
