/* Pager.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Page tables 'n stuff.
 */

#ifndef __MEMORY_PAGE_HH__
#define __MEMORY_PAGE_HH__

#include "StartupInformation.hh"
#include "kstd/Types.hh"

namespace kernel {

struct PageDirectoryEntry;

/**
 * Handles allocating pages
 */
struct PageAllocator
{
    void initialize(const StartupInformation& startupInformation, void* pageDirectory);

    // TODO: A method to install a page table into the system. For per-process page tables, I'll need a way to set the current page table so the system knows where to look.

private:
    static const u16 NumberOfEntries;

    /** The kernel's page directory. */
    PageDirectoryEntry* mPageDirectory;
};

} /* namespace kernel */

#endif /* __MEMORY_PAGE_HH__ */
