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

} /* namespace kernel */
