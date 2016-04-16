/* FrameAllocator.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * This file deals with page frames, chunks of physical memory of `pageSize`
 * length that can be filled with pages.
 */

#ifndef __MEMORY_FRAMEALLOCATOR_HH__
#define __MEMORY_FRAMEALLOCATOR_HH__

#include "StartupInformation.hh"
#include "kstd/Bitmap.hh"
#include "kstd/Types.hh"

namespace kernel {

struct FrameAllocator
{
    FrameAllocator();

    void initialize(const StartupInformation& startupInformation);

    /**
     * Allocate a page frame. Find a free page frame, mark it in use, and return
     * its address.
     */
    void* allocate();

private:
    typedef kstd::Bitmap<u8> Bitmap;

    /** Starting address of the frame allocation bitmap. */
    Bitmap* mBitmap;
    /** Size of the bitmap in `sizeof(Bitmap)` units. */
    u32 mBitmapSize;
    /** Total number of pages. */
    u32 mNumberOfPages;

    /** Reserve a range of memory. */
    void reserveRange(u32 start, u32 length);

    /** Return the physical memory address of `page`. */
    void* addressOfPage(usize page) const;
};

} /* namespace kernel */

#endif /* __MEMORY_FRAMEALLOCATOR_HH__ */

