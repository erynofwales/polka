/* FrameAllocator.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * An object to tracks and allocate physical page frames.
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

private:
    typedef kstd::Bitmap<u8> Bitmap;

    /** Starting address of the frame allocation bitmap. */
    Bitmap* mBitmap;
    /** Size of the bitmap in `sizeof(Bitmap)` units. */
    u32 mBitmapSize;

    /** Reserve a range of memory. */
    void reserveRange(u32 start, u32 length);
};

} /* namespace  */

#endif /* __MEMORY_FRAMEALLOCATOR_HH__ */

