/* PrintFormat.cc
 * vim: set tw=80:
 * Eryn Wells <eryn@erynwells.me>
 */
/**
 * Declares printFormat(), for writing formatted strings to the appropriate channel.
 */

#include <stdarg.h>
#include "Console.hh"
#include "Kernel.hh"
#include "kstd/ASCII.hh"
#include "kstd/CString.hh"
#include "kstd/Types.hh"

namespace {

struct Spec {
    enum class Size {
        Normal,
        DoubleShort,
        Short,
        Long,
        DoubleLong,
    };

    enum class Type {
        Char,
        Hex,
        Int,
        Pointer,
        String,
        Unsigned,
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
        void* p;
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
    char *str = nullptr;

    // Type::Char is a special case because it will always be a single character in length and there is no \0 to terminate it.
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

    if (type == Type::Int || type == Type::Hex || type == Type::Pointer || type == Type::Unsigned) {
        if (zeroPadded || type == Type::Pointer) {
            pad = '0';
        }
        if (type == Type::Int) {
            switch (size) {
                case Size::Normal:
                    length = kstd::CString::fromInteger(value.d, buf, 32);
                    break;
                case Size::DoubleShort:
                    length = kstd::CString::fromInteger(value.hhd, buf, 32);
                    break;
                case Size::Short:
                    length = kstd::CString::fromInteger(value.hd, buf, 32);
                    break;
                case Size::Long:
                    length = kstd::CString::fromInteger(value.ld, buf, 32);
                    break;
                case Size::DoubleLong:
                    length = kstd::CString::fromInteger(value.lld, buf, 32);
                    break;
            }
        } else if (type == Type::Hex || type == Type::Unsigned) {
            switch (size) {
                case Size::Normal:
                    length = kstd::CString::fromUnsignedInteger(value.x, buf, 32, 16, capitalized);
                    break;
                case Size::DoubleShort:
                    length = kstd::CString::fromUnsignedInteger(value.hhx, buf, 32, 16, capitalized);
                    break;
                case Size::Short:
                    length = kstd::CString::fromUnsignedInteger(value.hx, buf, 32, 16, capitalized);
                    break;
                case Size::Long:
                    length = kstd::CString::fromUnsignedInteger(value.lx, buf, 32, 16, capitalized);
                    break;
                case Size::DoubleLong:
                    length = kstd::CString::fromUnsignedInteger(value.llx, buf, 32, 16, capitalized);
                    break;
            }
        } else if (type == Type::Pointer) {
            length = kstd::CString::fromPointer(value.p, buf, 32, 16, true);
        }
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


inline bool
isSize(char c)
{
    return c == 'h' || c == 'l';
}


inline bool
isSpecifier(char c)
{
    return c == 'c' || c == 'd' || c == 'i' || c == 'p' || c == 's' || c == 'x' || c == 'X';
}

} /* anonymous namespace */


namespace kstd {

int
printFormat(const char* format,
            va_list args)
{
    enum {
        Default = 0,
        Percent,
        Width,
        Size
    } state = Default;

    auto& cons = kernel::Kernel::systemKernel().console();

    int nchars = 0;
    Spec spec;

    for (const char* p = format; *p != 0; p++) {
        switch (state) {
            case Default:
                if (*p == '%') {
                    state = Percent;
                    spec.clear();
                } else {
                    cons.printChar(*p);
                    nchars++;
                }
                break;
            case Percent:
                if (*p == '%') {
                    state = Default;
                    cons.printChar(*p);
                    nchars++;
                } else if (Char::isDigit(*p)) {
                    if (*p == '0' && !spec.zeroPadded) {
                        spec.zeroPadded = true;
                    } else {
                        state = Width;
                        spec.width = *p - '0';
                    }
                } else if (isSize(*p)) {
                    goto state_size;
                } else if (isSpecifier(*p)) {
                    goto state_specifier;
                }
                break;
            case Width:
                if (Char::isDigit(*p)) {
                    spec.width = 10 * spec.width + (*p - '0');
                } else if (isSize(*p)) {
                    state = Size;
                    goto state_size;
                } else if (isSpecifier(*p)) {
                    goto state_specifier;
                }
                break;
            case Size:
                if (isSize(*p)) {
                    goto state_size;
                } else if (isSpecifier(*p)) {
                    goto state_specifier;
                }
                break;
            default:
                cons.printChar(*p);
                nchars++;
                break;
        }
        continue;

    state_size:
        if (*p == 'h') {
            spec.size = (spec.size == Spec::Size::Short) ? Spec::Size::DoubleShort : Spec::Size::Short;
        } else if (*p == 'l') {
            spec.size = (spec.size == Spec::Size::Long) ? Spec::Size::DoubleLong : Spec::Size::Long;
        }
        continue;

    state_specifier:
        state = Default;
        switch (*p) {
            case 'c':
                spec.value.c = va_arg(args, int);
                spec.type = Spec::Type::Char;
                break;
            case 'd':
            case 'i':
                switch (spec.size) {
                    case Spec::Size::Normal:
                        spec.value.d = va_arg(args, int);
                        break;
                    case Spec::Size::DoubleShort:
                        spec.value.hhd = (signed char)va_arg(args, int);
                        break;
                    case Spec::Size::Short:
                        spec.value.hd = (short int)va_arg(args, int);
                        break;
                    case Spec::Size::Long:
                        spec.value.ld = va_arg(args, long int);
                        break;
                    case Spec::Size::DoubleLong:
                        spec.value.lld = va_arg(args, long long int);
                        break;
                }
                spec.type = Spec::Type::Int;
                break;
            case 'p':
                spec.value.p = va_arg(args, void*);
                spec.type = Spec::Type::Pointer;
                break;
            case 's':
                spec.value.s = va_arg(args, char*);
                spec.type = Spec::Type::String;
                break;
            case 'X':
                spec.capitalized = true;
                // fall through
            case 'x':
                switch (spec.size) {
                    case Spec::Size::Normal:
                        spec.value.x = va_arg(args, unsigned int);
                        break;
                    case Spec::Size::DoubleShort:
                        spec.value.hhx = (unsigned char)va_arg(args, unsigned int);
                        break;
                    case Spec::Size::Short:
                        spec.value.hx = (unsigned short int)va_arg(args, unsigned int);
                        break;
                    case Spec::Size::Long:
                        spec.value.lx = va_arg(args, unsigned long int);
                        break;
                    case Spec::Size::DoubleLong:
                        spec.value.llx = va_arg(args, unsigned long long int);
                        break;
                }
                spec.type = Spec::Type::Hex;
                break;
        }
        nchars += spec.print(cons);
        continue;
    }

    return nchars;
}


int
printFormat(const char* format,
            ...)
{
    va_list args;
    va_start(args, format);
    int nchars = printFormat(format, args);
    va_end(args);
    return nchars;
}

} /* namespace kstd */
