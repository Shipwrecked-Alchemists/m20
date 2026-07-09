#include "printf.h"

#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#include "main.h"

struct printf_ctx {
    enum {
        CTX_FILE,
        CTX_ARRAY
    } type;
    void* ptr;
    size_t size;
    size_t offset;
};

void usart_putc(uint8_t c) {
    LL_GPIO_SetOutputPin(OUT_LED_GPIO_Port, OUT_LED_Pin);
    while (!LL_LPUART_IsActiveFlag_TXE(USART1)) {}
    LL_LPUART_TransmitData8(USART1, c);
    LL_GPIO_ResetOutputPin(OUT_LED_GPIO_Port, OUT_LED_Pin);
}

static void write(struct printf_ctx *ctx, int c) {
    if (ctx->type == CTX_FILE) {
        usart_putc(c);
        ctx->offset++;
    } else if (ctx->type == CTX_ARRAY) {
        if (ctx->offset >= ctx->size - 1)
            return;
        ((char*)ctx->ptr)[ctx->offset++] = c;
    }
}

int n_printf(const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    int r = n_vprintf(format, ap);
    va_end(ap);
    return r;
}

int n_fprintf(FILE* stream, const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    int r = n_vfprintf(stream, format, ap);
    va_end(ap);
    return r;
}

int n_vprintf(const char* format, va_list ap) { return n_vfprintf(stdout, format, ap); }

int n_sprintf(char* str, const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    int r = n_vsnprintf(str, SIZE_MAX, format, ap);
    va_end(ap);
    return r;
}

int n_snprintf(char* str, size_t size, const char* format, ...) {
    va_list ap;
    va_start(ap, format);
    int r = n_vsnprintf(str, size, format, ap);
    va_end(ap);
    return r;
}

int n_vsprintf(char* str, const char* format, va_list ap) {
    return n_vsnprintf(str, SIZE_MAX, format, ap);
}

enum {
    F_LJUST = 0x1,
    F_SIGN = 0x2,
    F_SPACE = 0x4,
    F_ZERO = 0x8,

    F_WIDTH = 0x100,
    F_PRECISION = 0x200
};

enum {
    L_I = 0, // Default
    L_HH,
    L_H,
    L_L,
    L_LL,
    L_J,
    L_Z,
    L_T,
    // j z t L
};

static int itoa(char* buf, unsigned long long v, int base, int upper) {
    int len = 0;

    if (v == 0) {
        *buf = '0';
        len++;
    } else {
        while (v) {
            const char* hexdig_upper = "0123456789ABCDEF";
            const char* hexdig = "0123456789abcdef";
            *buf++ = upper ? hexdig_upper[v % base] : hexdig[v % base];
            len++;
            v /= base;
        }
    }
    return len;
}

static void pad_prec(int flags, int width, int precision, int len, int* pad, int* prec) {
    if ((flags & F_WIDTH) && width > len)
        *pad = width - len;

    if ((flags & F_PRECISION) && precision > len) {
        *prec = precision - len;
        *pad -= *prec;
    }
}

static void write_pad(struct printf_ctx *ctx, int pad, int padc) {
    for (int i = 0; i < pad; i++) {
        write(ctx, padc);
    }
}

static void write_number(struct printf_ctx *ctx, char* buf, int len, int prec) {
    for (int i = 0; i < prec; i++)
        write(ctx, '0');

    for (int i = len - 1; i >= 0; i--)
        write(ctx, buf[i]);
}

static void handle_signed(struct printf_ctx *ctx, va_list ap, int flags, int width,
                          int precision, int length) {
    long long v;
    switch (length) {
    case L_HH:
        v = (char)va_arg(ap, int);
        break;
    case L_H:
        v = (short)va_arg(ap, int);
        break;
    case L_I:
        v = va_arg(ap, int);
        break;
    case L_L:
        v = va_arg(ap, long);
        break;
    case L_LL:
        v = va_arg(ap, long long);
        break;
    case L_J:
        v = va_arg(ap, intmax_t);
        break;
    case L_Z:
        v = va_arg(ap, ssize_t);
        break;
    case L_T:
        v = va_arg(ap, ptrdiff_t);
        break;
    default:
        assert(0 && "Unknown length");
    }

    int neg = v < 0;
    if (neg)
        v = -v;

    char tmp[sizeof(long long) * 2];
    int len = itoa(tmp, v, 10, 0);

    int pad = 0, prec = 0;
    pad_prec(flags, width, precision, len, &pad, &prec);

    if (neg || flags & F_SIGN || flags & F_SPACE)
        pad--;

    char padc = (flags & F_ZERO) ? '0' : ' ';

    if (flags & F_SPACE && !neg)
        write(ctx, ' ');
    if (!(flags & F_SIGN) && neg && padc == '0')
        write(ctx, '-');

    if (flags & F_SIGN && padc == '0')
        write(ctx, neg ? '-' : '+');

    if (!(flags & F_LJUST))
        write_pad(ctx, pad, padc);

    if (flags & F_SIGN && padc == ' ')
        write(ctx, neg ? '-' : '+');
    if (!(flags & F_SIGN) && neg && padc == ' ')
        write(ctx, '-');

    write_number(ctx, tmp, len, prec);

    if (flags & F_LJUST)
        write_pad(ctx, pad, ' ');
}

static void handle_unsigned(struct printf_ctx *ctx, va_list ap, int flags, int width,
                            int precision, int length, char spec) {
    unsigned long long v;
    switch (length) {
    case L_HH:
        v = (unsigned char)va_arg(ap, unsigned int);
        break;
    case L_H:
        v = (unsigned short)va_arg(ap, unsigned int);
        break;
    case L_I:
        v = va_arg(ap, unsigned int);
        break;
    case L_L:
        v = va_arg(ap, unsigned long);
        break;
    case L_LL:
        v = va_arg(ap, unsigned long long);
        break;
    case L_J:
        v = va_arg(ap, uintmax_t);
        break;
    case L_Z:
    case L_T:
        v = va_arg(ap, size_t);
        break;
    default:
        assert(0 && "Unknown length");
    }

    char tmp[sizeof(unsigned long long) * 2];

    int len = 0;
    if (spec == 'u')
        len = itoa(tmp, v, 10, 0);
    else if (spec == 'o')
        len = itoa(tmp, v, 8, 0);
    else if (spec == 'x')
        len = itoa(tmp, v, 16, 0);
    else if (spec == 'X')
        len = itoa(tmp, v, 16, 1);
    else
        return;

    int pad = 0, prec = 0;
    pad_prec(flags, width, precision, len, &pad, &prec);

    char padc = (flags & F_ZERO) ? '0' : ' ';

    if (!(flags & F_LJUST))
        write_pad(ctx, pad, padc);
    write_number(ctx, tmp, len, prec);
    if (flags & F_LJUST)
        write_pad(ctx, pad, ' ');
}

static void handle_string(struct printf_ctx *ctx, char* s, int flags, int width,
                          int precision) {
    if (!s) {
        if (!(flags & F_PRECISION) || precision >= 6)
            s = "(null)";
        else
            s = "";
    }

    int len = strlen(s);
    if ((flags & F_PRECISION) && precision < len)
        len = precision;

    int pad = 0;
    if ((flags & F_WIDTH) && width > len)
        pad = width - len;

    if (!(flags & F_LJUST))
        write_pad(ctx, pad, ' ');
    while (len--)
        write(ctx, *s++);
    if (flags & F_LJUST)
        write_pad(ctx, pad, ' ');
}

static void handle_pointer(struct printf_ctx *ctx, void* ptr, int flags, int width,
                           int precision) {
    if (!ptr) {
        handle_string(ctx, "(nil)", flags & ~F_PRECISION, width, 0);
    }

    uintptr_t v = (uintptr_t)ptr;

    char tmp[sizeof(uintptr_t) * 2];
    int len = itoa(tmp, v, 16, 0);

    int pad = 0, prec = 0;
    pad_prec(flags, width, precision, len, &pad, &prec);

    if (flags & F_SIGN || flags & F_SPACE)
        pad--;

    char padc = !(flags & F_ZERO) || (flags & F_PRECISION) ? ' ' : '0';
    if (flags & F_SPACE)
        write(ctx, ' ');

    if (padc == '0') {
        if (flags & F_SIGN)
            write(ctx, '+');
        write(ctx, '0');
        write(ctx, 'x');
    }
    if (!(flags & F_LJUST))
        write_pad(ctx, pad - 2, padc);
    if (padc == ' ') {
        if (flags & F_SIGN)
            write(ctx, '+');
        write(ctx, '0');
        write(ctx, 'x');
    }

    write_number(ctx, tmp, len, prec);
    if (flags & F_LJUST)
        write_pad(ctx, pad - 2, ' ');
}

static void handle_char(struct printf_ctx *ctx, int c, int flags, int width) {
    if (!(flags & F_LJUST))
        write_pad(ctx, width - 1, ' ');
    write(ctx, c);
    if (flags & F_LJUST)
        write_pad(ctx, width - 1, ' ');
}

// TODO: Handle NaN, inf + rounding
static void handle_float(struct printf_ctx *ctx, double v, int flags, int width,
                         int precision) {
    if (!precision && !(flags& F_PRECISION)) precision = 6;

    int neg = v < 0;
    if (neg) v = -v;

    unsigned long long ip = (unsigned long long)v;
    double fp = v - (double)ip;

    char tmp_int[sizeof(long long)*2];
    int len_int = itoa(tmp_int, ip, 10, 0);

    int len = len_int + precision + (neg ? 1 : 0);
    if (flags & F_PRECISION && precision)
        len++;
    int pad = width > len ? width - len : 0;
    char padc = (flags & F_ZERO) ? '0' : ' ';

    if (!(flags & F_LJUST))
        write_pad(ctx, pad, padc);
    if (flags & F_SPACE)
        write(ctx, ' ');
    if (flags & F_SIGN || neg)
        write(ctx, neg ? '-' : '+');

    // Integer part
    write_number(ctx, tmp_int, len_int, 0);

    // Fractional part
    if (precision) {
        write(ctx, '.');
        while (precision--) {
            int int_part = 10*fp;
            write(ctx, int_part + '0');
            fp *= 10;
            fp -= int_part;
        }
    }


    if (flags & F_LJUST)
        write_pad(ctx, pad, ' ');
}

static void printf_format(struct printf_ctx *ctx, const char* format, va_list ap) {
    while (*format) {
        if (*format != '%') {
            write(ctx, *format++);
            continue;
        }
        format++;
        if (*format == '%') {
            write(ctx, *format++);
            continue;
        }

        int flags = 0;
        long width = 0;
        long precision = 0;
        int length = L_I;

        // Parse flags
        for (;;) {
            switch (*format) {
            case '-':
                flags |= F_LJUST;
                break;
            case '+':
                flags |= F_SIGN;
                break;
            case ' ':
                flags |= F_SPACE;
                break;
            case '0':
                flags |= F_ZERO;
                break;
            case '#':
                break; // TODO:
            default:
                goto flags_done;
            }
            format++;
        }
    flags_done:
        if (flags & F_LJUST)
            flags &= ~F_ZERO; // '0' ignored when '-' is present
        if (flags & F_SIGN)
            flags &= ~F_SPACE; // ' ' ignored when sign is present

        // Parse width
        if (*format == '*') {
            flags |= F_WIDTH;
            width = va_arg(ap, int);
            format++;
        } else if (isdigit(*format)) {
            flags |= F_WIDTH;
            while (isdigit(*format)) {
                const char c = *format;
                width = (width * 10) + (c - '0');
                format++;
            }
        }

        // Parse precision
        if (*format == '.') {
            flags |= F_PRECISION;
            format++;
            if (*format == '*') {
                precision = va_arg(ap, int);
                format++;
            } else if (isdigit(*format)) {
                while (isdigit(*format)) {
                    const char c = *format;
                    precision = (precision * 10) + (c - '0');
                    format++;
                }
            }
        }

        if (*format == 'h') {
            length = L_H;
            format++;
            if (*format == 'h') {
                length = L_HH;
                format++;
            }
        } else if (*format == 'l') {
            length = L_L;
            format++;
            if (*format == 'l') {
                length = L_LL;
                format++;
            }
        } else if (*format == 'j') {
            length = L_J;
            format++;
        } else if (*format == 'z') {
            length = L_Z;
            format++;
        }else if (*format == 't') {
            length = L_T;
            format++;
        }

        if (
            (flags & F_WIDTH && width > INT_MAX)
                || (flags & F_PRECISION && precision > INT_MAX)) {
            ctx->offset = -1;
            return;
        }

        // Parse specifier
        char spec = *format;
        switch (spec) {
        case 'd':
        case 'i':
            handle_signed(ctx, ap, flags, width, precision, length);
            break;
        case 'o':
        case 'u':
        case 'x':
        case 'X':
            handle_unsigned(ctx, ap, flags, width, precision, length, spec);
            break;
        case 'p':
            handle_pointer(ctx, va_arg(ap, void*), flags, width, precision);
            break;
        case 'c':
            // TODO: l modifier
            handle_char(ctx, va_arg(ap, int), flags, width);
            break;
        case 's':
            // TODO: l modifier
            handle_string(ctx, va_arg(ap, char*), flags, width, precision);
            break;
        case 'f':
        case 'F':
            handle_float(ctx, va_arg(ap, double), flags, width, precision);
            break;

        case 'e':
        case 'E':
        case 'g':
        case 'G':
        case 'a':
        case 'A':
        case 'n':
            // TODO:
            break;
        default:
            write(ctx, *format);
        }
        format++;
    }
}


int n_vfprintf(FILE* stream, const char* format, va_list ap) {
    struct printf_ctx ctx = {
        .type = CTX_FILE,
        .ptr = stream,
        .offset = 0
    };
    printf_format(&ctx, format, ap);
    return ctx.offset;
}

int n_vsnprintf(char* str, size_t size, const char* format, va_list ap) {
    struct printf_ctx ctx = {
        .type = CTX_ARRAY,
        .ptr = str,
        .size = size,
        .offset = 0
    };
    printf_format(&ctx, format, ap);

    ((char*)ctx.ptr)[ctx.offset] = 0;
    return ctx.offset;
}
