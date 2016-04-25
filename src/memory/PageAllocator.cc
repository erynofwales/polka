/* PageAllocator.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Page tables 'n stuff.
 */

#include "Attributes.hh"
#include "kstd/Bitmap.hh"
#include "kstd/Memory.hh"
#include "kstd/PrintFormat.hh"
#include "memory/Memory.hh"
#include "memory/PageAllocator.hh"

namespace kernel {

struct PageEntry
{
    PageEntry();

    enum class Present { No = 0, Yes = 1 };
    enum class ReadWrite { No = 0, Yes = 1 };
    enum class UserAccess { No = 0, Yes = 1 };

    void set(Present present);
    void set(ReadWrite readWrite);
    void set(UserAccess user);

    void setAddress(void* address);

protected:
    struct Flag {
        static const u8 Present             = 0;
        static const u8 ReadWrite           = 1;
        static const u8 UserAccess          = 2;
        static const u8 PageWriteThrough    = 3;
        static const u8 PageCacheDisabled   = 4;
        static const u8 Accessed            = 5;
        static const u8 Dirty               = 6;
        static const u8 PageAttributeTable  = 7;
        static const u8 Global              = 8;
    };

    static const u32 AddressMask         = 0xFFFFF000;

    u32 mEntry;
};

PageEntry::PageEntry()
    : mEntry(0)
{
    static_assert(sizeof(PageEntry) == 4, "PageEntry must be 4 bytes long.");
}

void
PageEntry::set(Present present)
{
    if (present == Present::Yes) {
        kstd::Bit::set(mEntry, Flag::Present);
    } else {
        kstd::Bit::clear(mEntry, Flag::Present);
    }
}

void
PageEntry::set(ReadWrite rw)
{
    if (rw == ReadWrite::Yes) {
        kstd::Bit::set(mEntry, Flag::ReadWrite);
    } else {
        kstd::Bit::clear(mEntry, Flag::ReadWrite);
    }
}

void
PageEntry::set(UserAccess user)
{
    if (user == UserAccess::Yes) {
        kstd::Bit::set(mEntry, Flag::UserAccess);
    } else {
        kstd::Bit::clear(mEntry, Flag::UserAccess);
    }
}

void
PageEntry::setAddress(void* address)
{
    kstd::Bit::setMask(mEntry, memory::pageAlignDown(uptr(address)), AddressMask);
}


struct PageDirectoryEntry
    : public PageEntry
{
    void setFlagsForSystemDirectory();
};

void
PageDirectoryEntry::setFlagsForSystemDirectory()
{
    set(PageEntry::Present::Yes);
    set(PageEntry::ReadWrite::Yes);
    set(PageEntry::UserAccess::No);
}


struct PageTableEntry
    : public PageEntry
{ };

/*
 * Public
 */

void
PageAllocator::initialize(const StartupInformation& startupInformation,
                          FrameAllocator* frameAllocator)
{
    mPageDirectory = reinterpret_cast<PageDirectoryEntry*>(frameAllocator->allocate());
    kstd::printFormat("Page directory at 0x%08lX\n", uptr(mPageDirectory));
    kstd::Memory::zero(mPageDirectory, memory::pageSize * (memory::pageSize / sizeof(PageDirectoryEntry)));

    auto& firstPageTable = mPageDirectory[0];
    firstPageTable.setAddress(frameAllocator->allocate());
    firstPageTable.setFlagsForSystemDirectory();
}

/*
 * Private
 */

const u16 PageAllocator::NumberOfEntries = 1024;


} /* namespace kernel */
