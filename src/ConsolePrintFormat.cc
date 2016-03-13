/* ConsolePrintFormat.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Implementation of Console::printFormat, a printf style method.
 */

#include <stdarg.h>

void itoa(int value, char* buffer, int base);


struct PrintfSpec
{
    enum Size {
        SizeNormal,
        SizeDoubleShort,
        SizeShort,
        SizeLong,
        SizeDoubleLong,
    };

    enum Type {
        TypeInt,
        TypeHex,
        TypeChar,
        TypeString
    };

    union Value {
        int d;
        char hhd;
        short int hd;
        long int ld;
        long long int lld;
        unsigned int x;
        unsigned char hhx;
        unsigned short int hx;
        unsigned long int lx;
        unsigned long long int llx;
        char c;
        char* s;
    };

    bool zero;
    int width;
    Size size;
    Type type;
    Value value;

    void clear();
    int print(Console& console);
};


void
PrintfSpec::clear()
{
    zero = false;
    width = 0;
    size = PrintfSpec::SizeNormal;
    type = PrintfSpec::TypeInt;
}


int
PrintfSpec::print(Console& console)
{
    int nchars = 0;
    int length = 0;
    char buf[32];
    char pad = ' ';
    char *str;

    /*
     * TypeChar is a special case because it will always be a single character
     * in length and there is no \0 to terminate it.
     */
    if (type == TypeChar) {
        if (width == 0) {
            width = 1;
        }
        for (int i = 1; i < width; i++) {
            putchar(' ');
        }
        putchar(value.c);
        return width;
    }

    if (type == TypeInt || type == TypeHex) {
        if (zero) {
            pad = '0';
        }
        if (type == TypeInt) {
            switch (size) {
                case SizeNormal:
                    itoa(value.d, buf, 10);
                    break;
                case SizeDoubleShort:
                    itoa(value.hhd, buf, 10);
                    break;
                case SizeShort:
                    itoa(value.hd, buf, 10);
                    break;
                case SizeLong:
                    itoa(value.ld, buf, 10);
                    break;
                case SizeDoubleLong:
                    itoa(value.lld, buf, 10);
                    break;
            }
        } else {
            switch (size) {
                case SizeNormal:
                    itoa(value.x, buf, 16);
                    break;
                case SizeDoubleShort:
                    itoa(value.hhx, buf, 16);
                    break;
                case SizeShort:
                    itoa(value.hx, buf, 16);
                    break;
                case SizeLong:
                    itoa(value.lx, buf, 16);
                    break;
                case SizeDoubleLong:
                    itoa(value.llx, buf, 16);
                    break;
            }
        }
        length = strlen(buf);
        if (width < length) {
            width = length;
        }
        str = buf;
    } else if (type == TypeString) {
        length = strlen(value.s);
        if (width < length) {
            width = length;
        }
        str = value.s;
    } else {
        // Don't know how to print this.
        return 0;
    }

    for (int i = width; i > 0; i--) {
        if (i <= length) {
            console.printChar(str[length - i]);
        }
        else if (i <= width) {
            console.printChar(pad);
        }
    }

    return nchars;
}


void
itoa(int value,
     char* buffer,
     int base)
{
    const bool neg = base == 10 && value < 0;

    if (neg) {
        value *= -1;
    }

    char* p = buffer;
    int place;
    do {
        place = value % base;
        value /= base;
        *p++ = place < 10 ? place + '0' : (place - 10) + 'a';
    } while (value != 0);

    if (neg) {
        *p++ = '-';
    }

    *p-- = '\0';

    // Reverse the string.
    char* t = buffer;
    char c;
    while (p > t) {
        c = *p;
        *p-- = *t;
        *t++ = c;
    }
}


void
to_upper(char* buffer)
{
    char* p = buffer;
    for (char* p = buffer; *p != 0; p++) {
        if (*p >= 'a' && *p <= 'z') {
            *p = (*p - 'a') + 'A';
        }
    }
}


inline bool
is_digit(char c)
{
    return c >= '0' && c <= '9';
}


inline bool
is_hex(char c)
{
    return is_digit(c) || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}


inline bool
is_lower(char c)
{
    return c >= 'a' && c <= 'z';
}


inline bool
is_upper(char c)
{
    return c >= 'A' && c <= 'Z';
}


inline bool
is_size(char c)
{
    return c == 'h' || c == 'l';
}


inline bool
is_specifier(char c)
{
    return c == 'd' || c == 'x' || c == 'X' || c == 'c' || c == 's';
}


inline bool
is_letter(char c)
{
    return is_lower(c) || is_upper(c);
}

namespace kernel {
    
int
Console::printFormat(const char* fmt,
                     ...)
{
    enum {
        StateDefault = 0,
        StatePercent,
        StateWidth,
        StateSize
    } state = StateDefault;

    va_list vl;
    va_start(vl, fmt);

    int nchars = 0;
    PrintfSpec spec;

    for (const char* p = fmt; *p != 0; p++) {
        switch (state) {
            case StateDefault:
                if (*p == '%') {
                    state = StatePercent;
                    spec.clear();
                } else {
                    printChar(*p);
                    nchars++;
                }
                break;
            case StatePercent:
                if (*p == '%') {
                    state = StateDefault;
                    printChar(*p);
                    nchars++;
                } else if (is_digit(*p)) {
                    if (*p == '0' && !spec.zero) {
                        spec.zero = true;
                    } else {
                        state = StateWidth;
                        spec.width = *p - '0';
                    }
                } else if (is_size(*p)) {
                    goto state_size;
                } else if (is_specifier(*p)) {
                    goto state_specifier;
                }
                break;
            case StateWidth:
                if (is_digit(*p)) {
                    spec.width = 10 * spec.width + (*p - '0');
                } else if (is_size(*p)) {
                    state = StateSize;
                    goto state_size;
                } else if (is_specifier(*p)) {
                    goto state_specifier;
                }
                break;
            case StateSize:
                if (is_size(*p)) {
                    goto state_size;
                } else if (is_specifier(*p)) {
                    goto state_specifier;
                }
                break;
            default:
                printChar(*p);
                nchars++;
                break;
        }
        continue;

    state_size:
        if (*p == 'h') {
            spec.size =   (spec.size == PrintfSpec::SizeShort)
                        ? PrintfSpec::SizeDoubleShort
                        : PrintfSpec::SizeShort;
        } else if (*p == 'l') {
            spec.size =   (spec.size == PrintfSpec::SizeLong)
                        ? PrintfSpec::SizeDoubleLong
                        : PrintfSpec::SizeLong;
        }
        continue;

    state_specifier:
        state = StateDefault;
        switch (*p) {
            case 'd':
            case 'i':
                switch (spec.size) {
                    case PrintfSpec::SizeNormal:
                        spec.value.d = va_arg(vl, int);
                        break;
                    case PrintfSpec::SizeDoubleShort:
                        spec.value.hhd = (signed char)va_arg(vl, int);
                        break;
                    case PrintfSpec::SizeShort:
                        spec.value.hd = (short int)va_arg(vl, int);
                        break;
                    case PrintfSpec::SizeLong:
                        spec.value.ld = va_arg(vl, long int);
                        break;
                    case PrintfSpec::SizeDoubleLong:
                        spec.value.lld = va_arg(vl, long long int);
                        break;
                }
                spec.type = PrintfSpec::TypeInt;
                break;
            case 'x':
            case 'X':
                switch (spec.size) {
                    case PrintfSpec::SizeNormal:
                        spec.value.x = va_arg(vl, unsigned int);
                        break;
                    case PrintfSpec::SizeDoubleShort:
                        spec.value.hhx = (unsigned char)va_arg(vl, unsigned int);
                        break;
                    case PrintfSpec::SizeShort:
                        spec.value.hx = (unsigned short int)va_arg(vl, unsigned int);
                        break;
                    case PrintfSpec::SizeLong:
                        spec.value.lx = va_arg(vl, unsigned long int);
                        break;
                    case PrintfSpec::SizeDoubleLong:
                        spec.value.llx = va_arg(vl, unsigned long long int);
                        break;
                }
                spec.type = PrintfSpec::TypeHex;
                break;
            case 'c':
                spec.value.c = va_arg(vl, int);
                spec.type = PrintfSpec::TypeChar;
                break;
            case 's':
                spec.value.s = va_arg(vl, char*);
                spec.type = PrintfSpec::TypeString;
                break;
        }
        nchars += spec.print(*this);
        continue;
    }

    va_end(vl);

    return nchars;
}

} /* namespace kernel */
