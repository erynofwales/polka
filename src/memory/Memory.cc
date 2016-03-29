/* Memory.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Top-level classes for managing system memory.
 */

#include "Memory.hh"
#include "kstd/PrintFormat.hh"

namespace kernel {

/*
 * Public
 */

void
MemoryManager::initialize()
{
    initializeGDT();
    kstd::printFormat("GDT loaded\n");
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
}

} /* namespace kernel */
