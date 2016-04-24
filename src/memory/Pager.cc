/* Pager.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Page tables 'n stuff.
 */

#include "Attributes.hh"
#include "Pager.hh"
#include "kstd/Bitmap.hh"
#include "memory/Memory.hh"

namespace kernel {

struct PageDirectoryEntry
{
    PageDirectoryEntry();

    /** Mark the whole entry not present. */
    void setNotPresent();
    bool isNotPresent() const;

    void setAddress(void* address);

private:
    static const u8 OneBit               = 0;
    static const u8 ReadWriteBit         = 1;
    static const u8 UserAccessBit        = 2;
    static const u8 PageWriteThroughBit  = 3;
    static const u8 PageCacheDisabledBit = 4;
    static const u8 AccessedBit          = 5;
    static const u8 DirtyBit             = 6;
    static const u8 ZeroBit              = 7;
    static const u32 IgnoredMask         = 0x00000F00;
    static const u32 AddressMask         = 0xFFFFF000;
    static const u32 FlagMask            = 0x00000FFF;

    u32 mEntry;

    void setRequiredBits();
};


PageDirectoryEntry::PageDirectoryEntry()
    : mEntry(0)
{
    static_assert(sizeof(PageDirectoryEntry) == 4, "PageDirectoryEntry must be 4 bytes long.");
}


bool
PageDirectoryEntry::isNotPresent()
    const
{
    return mEntry == 0;
}


void
PageDirectoryEntry::setNotPresent()
{
    mEntry = 0;
}


void
PageDirectoryEntry::setAddress(void* address)
{
    kstd::Bit::setMask(mEntry, memory::pageAlignDown(uptr(address)), AddressMask);
}

void
PageDirectoryEntry::setRequiredBits()
{
    kstd::Bit::set(mEntry, OneBit);
    kstd::Bit::clear(mEntry, ZeroBit);
    kstd::Bit::clearMask(mEntry, IgnoredMask);
}

/*
 * Public
 */

void
PageAllocator::initialize(const StartupInformation& startupInformation,
                          void* pageDirectory)
{
    mPageDirectory = reinterpret_cast<PageDirectoryEntry*>(pageDirectory);
    for (usize i = 0; i < NumberOfEntries; i++) {
        mPageDirectory[i].setNotPresent();
    }
}

/*
 * Private
 */

const u16 PageAllocator::NumberOfEntries = 1024;


} /* namespace kernel */
