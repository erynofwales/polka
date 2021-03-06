/* Console.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Implementation of the Console class.
 */

#include "Console.hh"

namespace kernel {

/** Create a VGA color pair. */
static inline uint8_t
makeVGAColor(Console::Color fg,
             Console::Color bg)
{
    return uint8_t(fg) | uint8_t(bg) << 4;
}


/** Create a VGA character entry. */
static inline uint16_t
makeVGAEntry(char c,
             uint8_t color)
{
    uint16_t c16 = c;
    uint16_t color16 = color;
    return c16 | color16 << 8;
}

/*
 * Static
 */

Console&
Console::systemConsole()
{
    static Console sSystemConsole;
    return sSystemConsole;
}

/*
 * Public
 */

Console::Console()
    : mBase(reinterpret_cast<uint16_t *>(0xB8000)),
      mCursor{0, 0},
      mColor(makeVGAColor(Console::Color::LightGray, Console::Color::Black))
{ }


void
Console::clear()
{
    for (size_t y = 0; y < Console::Height; y++) {
        for (size_t x = 0; x < Console::Width; x++) {
            putEntryAt(x, y, ' ', mColor);
        }
    }
    mCursor = {0, 0};
}


void
Console::clear(Console::Color color)
{
    setColor(Color::LightGray, color);
    clear();
}


void
Console::printChar(char c)
{
    switch (c) {
        case '\n':
            mCursor.col = 0;
            if (++mCursor.row >= Console::Height) {
                scroll();
                mCursor.row = Console::Height - 1;
            }
            break;
        case '\t':
            mCursor.col += 8;
            if (mCursor.col >= Console::Width) {
                if (++mCursor.row >= Console::Height) {
                    scroll();
                    mCursor.row = Console::Height - 1;
                }
                mCursor.col %= Console::Width;
            }
            break;
        default:
            putEntryAt(mCursor.col, mCursor.row, c, mColor);
            if (++mCursor.col == Console::Width) {
                mCursor.col = 0;
                if (++mCursor.row >= Console::Height) {
                    scroll();
                    mCursor.row = Console::Height - 1;
                }
            }
            break;
    }
}

void
Console::printString(const char *str)
{
    // XXX: THIS IS VERY UNSAFE. PUT DOWN THE POINTER ERYN. NO BAD ERYN DONT YOU DARE.
    const char *cur = str;
    while (*cur != '\0') {
        printChar(*cur++);
    }
}

void
Console::setColor(Console::Color fg,
                  Console::Color bg)
{
    mColor = makeVGAColor(fg, bg);
}

/*
 * Private
 */

void
Console::putEntryAt(size_t x,
                    size_t y,
                    char c,
                    uint8_t color)
{
    const size_t index = y * Console::Width + x;
    mBase[index] = makeVGAEntry(c, color);
}

void
Console::scroll(size_t lines)
{
    if (lines == 0) {
        return;
    }

    if (lines > Console::Height) {
        clear();
        return;
    }

    uint16_t *dst = mBase;
    uint16_t *src = mBase + lines * Console::Width;
    uint16_t *const end = mBase + Console::Width * Console::Height;
    while (src < end) {
        *dst++ = *src++;
    }
    while (dst < end) {
        *dst++ = makeVGAEntry(' ', mColor);
    }
}

} /* namespace kernel */
