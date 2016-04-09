/* StartupInformation.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Definition of the StartupInformation struct.
 */

#include "StartupInformation.hh"

namespace kernel {

/*
 * Public
 */

u32
StartupInformation::kernelSize()
    const
{
    return kernelEnd - kernelStart;
}

u32
StartupInformation::memorySize()
    const
{
    // Memory is 1 MiB plus however much upper memory we have.
    return 1024 * 1024 + multibootInformation->upperMemoryKB() * 1024;
}

} /* namespace kernel */
