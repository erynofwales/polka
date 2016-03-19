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
    
} /* namespace multiboot */
