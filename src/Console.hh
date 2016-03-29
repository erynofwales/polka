/* Console.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Declaration of the Console class. Presents an API for a VGA console.
 */

#ifndef __CONSOLE_HH__
#define __CONSOLE_HH__

#include <stdarg.h>
#include <stddef.h>
#include <stdint.h>
#include "Attributes.hh"


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

    static Console &systemConsole();

    Console();

    /** Clear the console to the current color. */
    void clear();
    /** Clear the console to the provided color. */
    void clear(Color color);

    /** Write a character to the terminal at the current cursor position. */
    void printChar(char c);

    /** Write a string to the terminal at the current cursor position. */
    void printString(const char *str);

    /**
     * Set the current cursor color. Subsequent characters will be written in
     * this color.
     */
    void setColor(Color fg, Color bg);

private:
    uint16_t *const mBase;
    Cursor mCursor;
    uint8_t mColor;

    Console(const Console& other) = delete;
    Console& operator=(const Console& other) = delete;

    void putEntryAt(size_t x, size_t y, char c, uint8_t color);
    void scroll(size_t lines = 1);
};

} /* namespace kernel */

#endif /* __CONSOLE_HH__ */
