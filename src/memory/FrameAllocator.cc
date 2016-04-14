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
#include "memory/Memory.hh"

namespace kernel {

FrameAllocator::FrameAllocator()
    : mBitmap(nullptr),
      mBitmapSize(0)
{ }


void
FrameAllocator::initialize(const StartupInformation& startupInformation)
{
    // Page frame bitmap starts immediately after the kernel.
    mBitmap = reinterpret_cast<Bitmap*>(startupInformation.kernelEnd);

    mNumberOfPages = startupInformation.memorySize() / memory::pageSize;
    const u32 pagesPerBitmap = Bitmap::length;

    mBitmapSize = mNumberOfPages / pagesPerBitmap;
    if ((mNumberOfPages % pagesPerBitmap) != 0) {
        // Add an extra bitmap for the last few pages.
        mBitmapSize++;
    }

    kstd::printFormat("Allocated bitmap of %ld bytes for %ld pages at 0x%08lX\n", mBitmapSize * sizeof(Bitmap), mNumberOfPages, u32(mBitmap));

    // TODO: Before modifying this memory, maybe make sure none of the multiboot information is hanging out there?

    kstd::Memory::zero(mBitmap, mBitmapSize);

    // Lower 1 MB is always allocated.
    reserveRange(0, 0x100000);
    // Kernel image (including this frame bitmap) is always allocated.
    reserveRange(startupInformation.kernelStart, startupInformation.kernelSize() + mBitmapSize);
}

void*
FrameAllocator::allocate()
{
    // Find the first bitmap with a free slot, and the first free slot, and return it.
    const u32 pagesPerBitmap = Bitmap::length;
    for (usize i = 0; i < mNumberOfPages; i++) {
        auto bitmap = mBitmap[i];
        if (bitmap.isFull()) {
            continue;
        }
        kstd::printFormat("Found partially full bitmap %ld -> %02X\n", i, u8(bitmap));
        for (usize j = 0; j < pagesPerBitmap; j++) {
            if (bitmap.isSet(j)) {
                continue;
            }
            kstd::printFormat("Found unset bit %ld\n", j);
            bitmap.set(j);
            usize page = i * pagesPerBitmap + j;
            void* pageAddress = addressOfPage(page);
            kstd::printFormat("Allocating frame for page %ld at address 0x%08lX\n", page, u32(pageAddress));
            return pageAddress;
        }
    }
    kstd::printFormat("Couldn't allocate frame\n");
    return nullptr;
}

void
FrameAllocator::reserveRange(u32 start,
                             u32 length)
{
    // TODO: `start` and `length` validation.
    const u32 pagesPerBitmap = Bitmap::length;

    const u32 startPage = memory::pageAlignDown(start) / memory::pageSize;
    const u32 endPage = memory::pageAlignUp(start + length) / memory::pageSize;

    // Iterators
    u32 page = startPage;
    u32 bitmapIndex = startPage / pagesPerBitmap;
    u8 bitmapOffset = startPage % pagesPerBitmap;

    kstd::printFormat("Reserving %ld pages for memory addresses between 0x%08lX and 0x%08lX\n", endPage - startPage, start, start + length);

    // Fill in any entries that aren't aligned to pagesPerBitmap at the beginning.
    while (bitmapOffset != 0) {
        mBitmap[bitmapIndex].set(bitmapOffset);
        bitmapOffset = (bitmapOffset + 1) % 8;
        page++;
        if (bitmapOffset == 0) {
            bitmapIndex++;
        }
    }

    // Fill in entries in `pagesPerBitmap` sized chunks.
    while ((endPage - page) > pagesPerBitmap) {
        mBitmap[bitmapIndex++].fill();
        page += pagesPerBitmap;
    }

    // Fill in any entries that aren't aligned to `pagesPerBitmap` at the end of the range.
    while ((endPage - page) > 0) {
        mBitmap[bitmapIndex].set(bitmapOffset);
        bitmapOffset = (bitmapOffset + 1) % 8;
        page++;
    }
}

inline void*
FrameAllocator::addressOfPage(usize page)
    const
{
    return reinterpret_cast<void*>(page * memory::pageSize);
}

} /* namespace kernel */
