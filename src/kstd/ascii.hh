/* ascii.hh
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * A few useful utility functions for dealing with ASCII values.
 */

namespace kernel {

struct Char
{
    /*
     * Categorization
     */

    static inline bool
    isDigit(char c)
    {
        return c >= '0' && c <='9';
    }

    static inline bool
    isHexDigit(char c)
    {
        return isDigit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
    }

    static inline bool
    isLower(char c)
    {
        return c >= 'a' && c <= 'z';
    }

    static inline bool
    isUpper(char c)
    {
        return c >= 'A' && c <= 'Z';
    }

    static inline bool
    isLetter(char c)
    {
        return isLower(c) || isUpper(c);
    }

    static inline bool
    isAlphanumeric(char c)
    {
        return isLetter(c) || isDigit(c);
    }

    /*
     * Conversion
     */

    static inline char
    toLower(char c)
    {
        if (isUpper(c)) {
            return 'a' + (c - 'A');
        }
        return c;
    }

    static inline char
    toUpper(char c)
    {
        if (isLower(c)) {
            return 'A' + (c - 'a');
        }
        return c;
    }
};


struct Int
{
    static void toString(int value, char *buffer, int base = 10);
};

} /* namespace kernel */
