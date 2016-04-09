/* FrameAllocator.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * An object to tracks and allocate physical page frames.
 */

#include "Kernel.hh"
#include "kstd/Memory.hh"
#include "kstd/PrintFormat.hh"
#include "memory/FrameAllocator.hh"

namespace kernel {

FrameAllocator::FrameAllocator()
    : mBitmap(nullptr),
      mBitmapSize(0)
{ }


void
FrameAllocator::initialize(const StartupInformation& startupInformation)
{
    // Page frame bitmap starts immediately after the kernel.
    mBitmap = reinterpret_cast<u8*>(startupInformation.kernelEnd);

    const u32 pageSize = startupInformation.pageSize;
    const u32 numberOfPages = startupInformation.multibootInformation->memoryKB() * 1024 / pageSize;
    mBitmapSize = numberOfPages / (sizeof(FrameBitmap) * 8);

    kstd::printFormat("Allocated bitmap of %ld bytes for %ld pages at 0x%08lX\n", mBitmapSize, numberOfPages, u32(mBitmap));

    // TODO: Before modifying this memory, maybe make sure none of the multiboot information is hanging out there?
    kstd::Memory::zero(mBitmap, mBitmapSize);
}

} /* namespace kernel */
