/* Console.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Implementation of the Console class.
 */

#include "Console.hh"

/** Create a VGA color pair. */
static inline uint8_t
makeVGAColor(Console::Color fg,
             Console::Color bg)
{
    return fg | bg << 4;
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


namespace kernel {

Console::Console()
    : mBase(0xB8000),
      mCursor({0, 0})
{ }


Console::clear(Console::Color color)
{
    const uint16_t color = makeVGAColor(Color::LightGray, Color::Blue);
    for (size_t y = 0; y < Console::Height; y++) {
        for (size_t x = 0; x < Console::Width; x++) {
            const size_t index = y * Console::Width + x;
            mBase[index] = makeVGAEntry(' ', color);
        }
    }
}

} /* namespace kernel */
