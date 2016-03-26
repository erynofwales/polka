/* Memory.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Declaration of top-level classes for managing system memory.
 */

#ifndef __MEMORY_HH__
#define __MEMORY_HH__

#include "Console.hh"
#include "Descriptors.hh"


namespace kernel {

struct MemoryManager
{
    void initialize(Console& console);

private:
    x86::GDT mGDT;

    void initializeGDT();
};

} /* namespace kernel */

#endif /* __MEMORY_HH__ */
