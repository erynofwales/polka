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

} /* namespace kernel */

#endif /* __MEMORY_MEMORY_HH__ */
