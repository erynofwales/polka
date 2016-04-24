/* Bitmap.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Declares a Bitmap object, which manages an array of bytes.
 */

#ifndef __KSTD_BITMAP_HH__
#define __KSTD_BITMAP_HH__

#include "kstd/Types.hh"

namespace kstd {

namespace Bit {

template<typename T>
inline bool
get(T field,
    u8 bit)
{
    return (field & (1 << bit)) > 0;
}

template<typename T>
inline void
set(T& field,
    u8 bit)
{
    field |= T(1 << bit);
}

template<typename T>
inline void
clear(T& field,
      u8 bit)
{
    field &= ~T(1 << bit);
}

template<typename T>
inline T
getMask(T field,
        T mask)
{
    return field & mask;
}

template<typename T>
inline void
setMask(T& field,
        T value,
        T mask)
{
    // Keep everything the same (~mask), but OR in the new value
    field = (field & ~mask) | value;
}

template<typename T>
inline void
clearMask(T& field,
          T mask)
{
    field &= ~mask;
}

} /* namespace Bit */

/** An array-like object of N bits, where N is the size of the type T given as a template parameter. */
template <typename T>
struct Bitmap
{
    typedef T FieldType;

    /** Size of the bitmap in bits. */
    static const usize length = sizeof(FieldType) * 8;

    Bitmap()
        : mBitmap(0)
    { }

    explicit
    Bitmap(FieldType value)
        : mBitmap(value)
    { }

    /** Get this bit field's primitive representation. */
    operator FieldType()
        const
    {
        return mBitmap;
    }

    /** Get the status of a single bit. Returns `true` if the bit is 1. */
    bool
    isSet(usize bit)
        const
    {
        return (mBitmap & FieldType(1 << bit)) > 0;
    }

    /** Set a single bit to 1. */
    void
    set(usize bit)
    {
        mBitmap |= FieldType(1 << bit);
    }

    /** Set a single bit to zero. */
    void
    clear(usize bit)
    {
        mBitmap &= ~FieldType(1 << bit);
    }

    /** Toggle the state of a single bit. Returns `true` if the bit was set to 1. */
    bool
    toggle(usize bit)
    {
        mBitmap ^= FieldType(1 << bit);
        return isSet(bit);
    }

    /** Clear the entire bit field. */
    void
    zero()
    {
        mBitmap = 0;
    }

    /** Set everything to 1. */
    void
    fill()
    {
        mBitmap = FieldType(-1);
    }

    bool
    isFull()
        const
    {
        return mBitmap == FieldType(-1);
    }

private:
    FieldType mBitmap;
};

} /* namespace kstd */

#endif /* __KSTD_BITMAP_HH__ */
