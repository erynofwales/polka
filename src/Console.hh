/* Console.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Declaration of the Console class. Presents an API for a VGA console.
 */

//#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

namespace kernel {

struct Console
{
    enum class Color {
        Black = 0,
        Blue = 1,
        Green = 2,
        Cyan = 3,
        Red = 4,
        Magenta = 5,
        Brown = 6,
        LightGray = 7,
        DarkGray = 8,
        LightBlue = 9,
        LightGreen = 10,
        LightCyan = 11,
        LightRed = 12,
        LightMagenta = 13,
        LightBrown = 14,
        White = 15
    };

    struct Cursor
    {
        size_t row, col;
    };

    static const size_t Width = 80;
    static const size_t Height = 25;

    Console();

    /** Clear the console to the provided color. */
    void clear(Color color = Color::Black);

    /** Write a character to the terminal at the current cursor position. */
    void writeChar(char c);

    /** Write a string to the terminal at the current cursor position. */
    void writeString(const char *str);

    void setColor(Color fg, Color bg);

private:
    uint16_t *const mBase;
    Cursor mCursor;
    uint8_t mColor;

    void putEntryAt(size_t x, size_t y, char c, uint8_t color);
};

} /* namespace kernel */
