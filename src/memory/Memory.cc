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
    auto multiboot = startupInformation.multibootInformation;
    kstd::printFormat("Memory map:\n");
    kstd::printFormat("  available: lower = %ld KB, upper = %ld KB, total = %ld KB\n",
                      multiboot->lowerMemoryKB(), multiboot->upperMemoryKB(),
                      startupInformation.memorySize() / 1024);
    for (auto it = multiboot->memoryMapBegin(); it != multiboot->memoryMapEnd(); ++it) {
        auto begin = (*it).base;
        auto end = begin + (*it).length - 1;
        kstd::printFormat("  begin = 0x%08lX %08lX, end = 0x%08lX %08lX (%s)\n",
                          u32(begin >> 32), u32(begin & 0xFFFFFFFF),
                          u32(end >> 32), u32(end & 0xFFFFFFFF),
                          (*it).type == 1 ? "available" : "reserved");
    }

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

const usize pageSize = 4096;
const usize pageMask = pageSize - 1;

} /* namespace memory */

} /* namespace kernel */
