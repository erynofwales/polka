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
                                                  uint32_t length)
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
        auto size = *reinterpret_cast<uint32_t*>(mCurrent) + 4;
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


Information::MemoryMapIterator
Information::memoryMapBegin()
    const
{
    if ((mFlags & Present::MemoryMap) == 0) {
        return memoryMapEnd();
    }
    auto& console = kernel::Console::systemConsole();
    return MemoryMapIterator(mMemoryMapAddress, mMemoryMapLength);
}


Information::MemoryMapIterator
Information::memoryMapEnd()
    const
{
    return {0, 0};
}

} /* namespace multiboot */
