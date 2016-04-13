/* Memory.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Top-level classes for managing system memory.
 */

#include "kstd/PrintFormat.hh"
#include "memory/Memory.hh"

namespace kernel {

/*
 * Public
 */

MemoryManager::MemoryManager()
    : mGDT(),
      mFrameAllocator()
{ }

void
MemoryManager::initialize(const StartupInformation& startupInformation)
{
    initializeGDT();
    mFrameAllocator.initialize(startupInformation);
}

/*
 * Private
 */

void
MemoryManager::initializeGDT()
{
    mGDT.setNullDescriptor(0);
    mGDT.setDescriptor(1, x86::GDT::DescriptorSpec::kernelSegment(0, 0xFFFFFFFF, x86::GDT::Type::CodeEXR));
    mGDT.setDescriptor(2, x86::GDT::DescriptorSpec::kernelSegment(0, 0xFFFFFFFF, x86::GDT::Type::DataRW));
    mGDT.load();
    kstd::printFormat("GDT loaded\n");
}

namespace memory {

const u32 pageSize = 4096;
const u32 pageMask = pageSize - 1;

} /* namespace memory */

} /* namespace kernel */
