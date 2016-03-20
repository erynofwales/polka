/* ConsolePrintFormat.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Implementation of Console::printFormat, a printf style method.
 */

#include <stdarg.h>
#include "Console.hh"
#include "kstd/ASCII.hh"
#include "kstd/CString.hh"
#include "kstd/Types.hh"

namespace {

void itoa(int value, char* buffer, int base);

struct Spec {
    enum class Size {
        Normal,
        DoubleShort,
        Short,
        Long,
        DoubleLong,
    };

    enum class Type {
        Int,
        Unsigned,
        Hex,
        Char,
        String
    };

    bool zeroPadded;
    bool capitalized;
    int width;
    Size size;
    Type type;

    union Value {
        i8 hhd;
        i16 hd;
        i32 d;
        i64 ld;
        i64 lld;
        u8 hhu;
        u16 hu;
        u32 u;
        u64 lu;
        u64 llu;
        u8 hhx;
        u16 hx;
        u32 x;
        u64 lx;
        u64 llx;
        char c;
        char* s;
    } value;

    void clear();
    int print(kernel::Console& console);
};


void
Spec::clear()
{
    zeroPadded = false;
    capitalized = false;
    width = 0;
    size = Size::Normal;
    type = Type::Int;
    value.llx = 0;
}


int
Spec::print(kernel::Console& console)
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
    if (type == Type::Char) {
        if (width == 0) {
            width = 1;
        }
        for (int i = 1; i < width; i++) {
            console.printChar(' ');
        }
        console.printChar(value.c);
        return width;
    }

    if (type == Type::Int || type == Type::Hex) {
        if (zeroPadded) {
            pad = '0';
        }
        if (type == Type::Int) {
            switch (size) {
                case Size::Normal:
                    itoa(value.d, buf, 10);
                    break;
                case Size::DoubleShort:
                    itoa(value.hhd, buf, 10);
                    break;
                case Size::Short:
                    itoa(value.hd, buf, 10);
                    break;
                case Size::Long:
                    itoa(value.ld, buf, 10);
                    break;
                case Size::DoubleLong:
                    itoa(value.lld, buf, 10);
                    break;
            }
        } else {
            switch (size) {
                case Size::Normal:
                    kstd::CString::fromUnsignedInteger(value.x, buf, 16, capitalized);
                    break;
                case Size::DoubleShort:
                    kstd::CString::fromUnsignedInteger(value.hhx, buf, 16, capitalized);
                    break;
                case Size::Short:
                    kstd::CString::fromUnsignedInteger(value.hx, buf, 16, capitalized);
                    break;
                case Size::Long:
                    kstd::CString::fromUnsignedLongInteger(value.lx, buf, 16, capitalized);
                    break;
                case Size::DoubleLong:
                    kstd::CString::fromUnsignedLongInteger(value.llx, buf, 16, capitalized);
                    break;
            }
        }
        length = kstd::CString::length(buf, 32);
        if (width < length) {
            width = length;
        }
        str = buf;
    } else if (type == Type::String) {
        length = kstd::CString::length(value.s);
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
    unsigned int v = (unsigned int)value;

    char* p = buffer;
    int place;
    do {
        place = v % base;
        v /= base;
        *p++ = place < 10 ? place + '0' : (place - 10) + 'a';
    } while (v != 0);

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

} /* anonymous namespace */

namespace kernel {

int
Console::printFormat(const char* fmt,
                     ...)
{
    enum {
        Default = 0,
        Percent,
        Width,
        Size
    } state = Default;

    va_list vl;
    va_start(vl, fmt);

    int nchars = 0;
    Spec spec;

    for (const char* p = fmt; *p != 0; p++) {
        switch (state) {
            case Default:
                if (*p == '%') {
                    state = Percent;
                    spec.clear();
                } else {
                    printChar(*p);
                    nchars++;
                }
                break;
            case Percent:
                if (*p == '%') {
                    state = Default;
                    printChar(*p);
                    nchars++;
                } else if (kstd::Char::isDigit(*p)) {
                    if (*p == '0' && !spec.zeroPadded) {
                        spec.zeroPadded = true;
                    } else {
                        state = Width;
                        spec.width = *p - '0';
                    }
                } else if (is_size(*p)) {
                    goto state_size;
                } else if (is_specifier(*p)) {
                    goto state_specifier;
                }
                break;
            case Width:
                if (kstd::Char::isDigit(*p)) {
                    spec.width = 10 * spec.width + (*p - '0');
                } else if (is_size(*p)) {
                    state = Size;
                    goto state_size;
                } else if (is_specifier(*p)) {
                    goto state_specifier;
                }
                break;
            case Size:
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
            spec.size =   (spec.size == Spec::Size::Short)
                        ? Spec::Size::DoubleShort
                        : Spec::Size::Short;
        } else if (*p == 'l') {
            spec.size =   (spec.size == Spec::Size::Long)
                        ? Spec::Size::DoubleLong
                        : Spec::Size::Long;
        }
        continue;

    state_specifier:
        state = Default;
        switch (*p) {
            case 'd':
            case 'i':
                switch (spec.size) {
                    case Spec::Size::Normal:
                        spec.value.d = va_arg(vl, int);
                        break;
                    case Spec::Size::DoubleShort:
                        spec.value.hhd = (signed char)va_arg(vl, int);
                        break;
                    case Spec::Size::Short:
                        spec.value.hd = (short int)va_arg(vl, int);
                        break;
                    case Spec::Size::Long:
                        spec.value.ld = va_arg(vl, long int);
                        break;
                    case Spec::Size::DoubleLong:
                        spec.value.lld = va_arg(vl, long long int);
                        break;
                }
                spec.type = Spec::Type::Int;
                break;
            case 'x':
            case 'X':
                switch (spec.size) {
                    case Spec::Size::Normal:
                        spec.value.x = va_arg(vl, unsigned int);
                        break;
                    case Spec::Size::DoubleShort:
                        spec.value.hhx = (unsigned char)va_arg(vl, unsigned int);
                        break;
                    case Spec::Size::Short:
                        spec.value.hx = (unsigned short int)va_arg(vl, unsigned int);
                        break;
                    case Spec::Size::Long:
                        spec.value.lx = va_arg(vl, unsigned long int);
                        break;
                    case Spec::Size::DoubleLong:
                        spec.value.llx = va_arg(vl, unsigned long long int);
                        break;
                }
                spec.type = Spec::Type::Hex;
                break;
            case 'c':
                spec.value.c = va_arg(vl, int);
                spec.type = Spec::Type::Char;
                break;
            case 's':
                spec.value.s = va_arg(vl, char*);
                spec.type = Spec::Type::String;
                break;
        }
        nchars += spec.print(*this);
        continue;
    }

    va_end(vl);

    return nchars;
}

} /* namespace kernel */
