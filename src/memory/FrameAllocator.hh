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
#include "kstd/Types.hh"


namespace kernel {

struct FrameAllocator
{
    FrameAllocator();

    void initialize(const StartupInformation& startupInformation);

private:
    typedef u8 FrameBitmap;

    /** Starting address of the frame allocation bitmap. */
    FrameBitmap* mBitmap;
    /** Size of the bitmap in bytes. */
    u32 mBitmapSize;
};

} /* namespace  */

#endif /* __MEMORY_FRAMEALLOCATOR_HH__ */

