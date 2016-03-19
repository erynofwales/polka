/* Multiboot.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Definition of multiboot stuff.
 */

#include "Multiboot.hh"

namespace {

enum Present {
    Memory = 1 << 0,
    BootDevice = 1 << 1,
    CommandLine = 1 << 2,
    Modules = 1 << 3,
    AOutSymbols = 1 << 4,
    ElfSymbols = 1 << 5,
    MemoryMap = 1 << 6,
    Drives = 1 << 7,
    ConfigurationTable = 1 << 8,
    BootLoaderName = 1 << 9,
    APMTable = 1 << 10,
    VBE = 1 << 11,
};

multiboot::Information *sInformation = 0;

}

namespace multiboot {

/*
 * Static
 */

const Information *
Information::information()
{
    return sInformation;
}


void
Information::setInformation(Information *info)
{
    sInformation = info;
}

/*
 * Public
 */

Information::MemoryMapIterator::MemoryMapIterator(uint32_t address,
                                                  uint32_t count)
    : mCurrent(reinterpret_cast<MemoryChunk *>(address)),
      mCount(count)
{ }


Information::MemoryMapIterator::MemoryMapIterator(const MemoryMapIterator& other)
    : mCurrent(other.mCurrent),
      mCount(other.mCount)
{ }


Information::MemoryMapIterator&
Information::MemoryMapIterator::operator++()
{
    if (mCount > 0) {
        auto p = reinterpret_cast<uint32_t *>(mCurrent);
        // The size of the current struct is stored at (mCurrent - 4) bytes.
        mCurrent = (mCurrent + p[-1]);
        mCount--;
    }
    return *this; 
}


Information::MemoryMapIterator
Information::MemoryMapIterator::operator++(int)
{
    MemoryMapIterator tmp(reinterpret_cast<uint32_t>(mCurrent), mCount);
    operator++();
    return tmp;
}


MemoryChunk
Information::MemoryMapIterator::operator*()
{
    if (mCount == 0) {
        return MemoryChunk{0, 0, 0};
    }
    return *mCurrent;
}


bool
Information::MemoryMapIterator::operator==(const MemoryMapIterator& other)
    const
{
    if (mCount == 0 && other.mCount == 0) {
        // If both iterators have 0 counts, it doesn't matter what the mCurrent members point at.
        return true;
    }
    return mCurrent == other.mCurrent && mCount == other.mCount;
}


bool
Information::MemoryMapIterator::operator!=(const MemoryMapIterator& other)
    const
{
    return !(*this == other);
}


uint32_t
Information::lowerMemoryKB()
    const
{
    if ((mFlags & Present::Memory) == 0) {
        return 0;
    }
    return mMemLower;
}


uint32_t
Information::upperMemoryKB()
    const
{
    if ((mFlags & Present::Memory) == 0) {
        return 0;
    }
    return mMemUpper;
}


const char *
Information::commandLine()
    const
{
    if ((mFlags & Present::CommandLine) == 0) {
        return 0;
    }
    return reinterpret_cast<const char *>(mCommandLine);
}


uint32_t
Information::memoryMapChunks()
    const
{
    if ((mFlags & Present::MemoryMap) == 0) {
        return 0;
    }
    return memoryMapCount;
}


Information::MemoryMapIterator
Information::memoryMapBegin()
    const
{
    if ((mFlags & Present::MemoryMap) == 0) {
        return memoryMapEnd();
    }
    return MemoryMapIterator(memoryMapAddress, memoryMapCount);
}


Information::MemoryMapIterator
Information::memoryMapEnd()
    const
{
    return {0, 0};
}
    
} /* namespace multiboot */
