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

}

namespace multiboot {

/*
 * Public
 */

Information::MemoryMapIterator::MemoryMapIterator(u32 address,
                                                  u32 length)
    : mCurrent(address),
      mLength(length)
{ }


Information::MemoryMapIterator::MemoryMapIterator(const MemoryMapIterator& other)
    : mCurrent(other.mCurrent),
      mLength(other.mLength)
{ }


Information::MemoryMapIterator&
Information::MemoryMapIterator::operator++()
{
    if (mLength > 0) {
        // The size of the current struct is stored at (mCurrent - 4) bytes.
        auto size = *reinterpret_cast<u32*>(mCurrent) + 4;
        mCurrent = mCurrent + size;
        mLength = (size > mLength) ? 0 : mLength - size;
    }
    return *this;
}


Information::MemoryMapIterator
Information::MemoryMapIterator::operator++(int)
{
    MemoryMapIterator tmp(mCurrent, mLength);
    operator++();
    return tmp;
}


MemoryChunk
Information::MemoryMapIterator::operator*()
{
    return *reinterpret_cast<MemoryChunk*>(mCurrent+4);
}


bool
Information::MemoryMapIterator::operator==(const MemoryMapIterator& other)
    const
{
    if (mLength == 0 && other.mLength == 0) {
        // If both iterators have 0 counts, it doesn't matter what the mCurrent members point at.
        return true;
    }
    return mCurrent == other.mCurrent && mLength == other.mLength;
}


bool
Information::MemoryMapIterator::operator!=(const MemoryMapIterator& other)
    const
{
    return !(*this == other);
}


u32
Information::lowerMemoryKB()
    const
{
    if ((mFlags & Present::Memory) == 0) {
        return 0;
    }
    return mMemLower;
}


u32
Information::upperMemoryKB()
    const
{
    if ((mFlags & Present::Memory) == 0) {
        return 0;
    }
    return mMemUpper;
}


u32
Information::memoryKB()
    const
{
    return lowerMemoryKB() + upperMemoryKB();
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


Information::MemoryMapIterator
Information::memoryMapBegin()
    const
{
    if ((mFlags & Present::MemoryMap) == 0) {
        return memoryMapEnd();
    }
    return MemoryMapIterator(mMemoryMapAddress, mMemoryMapLength);
}


Information::MemoryMapIterator
Information::memoryMapEnd()
    const
{
    return {0, 0};
}

} /* namespace multiboot */
