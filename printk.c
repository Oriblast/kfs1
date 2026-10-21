#include "printk.h"
#include "types.h"
#include "string.h"
#include "vga.h"

typedef __builtin_va_list va_list;
#define va_start(ap, last) __builtin_va_start(ap, last)
#define va_arg(ap, type)   __builtin_va_arg(ap, type)
#define va_end(ap)         __builtin_va_end(ap)

static int put_padded(const char* str, int width, char pad) {
    int len = (int)strlen(str);
    int count = 0;

    while (width-- > len) {
        terminal_putchar(pad);
        count++;
    }
    terminal_write(str);
    return count + len;
}

static char* utoa(uint32_t n, unsigned base, int upper, char* end) {
    const char* digits = upper ? "0123456789ABCDEF" : "0123456789abcdef";

    *end = '\0';
    do {
        *--end = digits[n % base];
        n /= base;
    } while (n);
    return end;
}

int printk(const char* fmt, ...) {
    va_list ap;
    char buf[12];
    int count = 0;

    va_start(ap, fmt);
    for (; *fmt; fmt++) {
        if (*fmt != '%') {
            terminal_putchar(*fmt);
            count++;
            continue;
        }
        fmt++;

        char pad = ' ';
        int width = 0;
        if (*fmt == '0') {
            pad = '0';
            fmt++;
        }
        while (*fmt >= '0' && *fmt <= '9')
            width = width * 10 + (*fmt++ - '0');

        switch (*fmt) {
        case 'c':
            buf[0] = (char)va_arg(ap, int);
            buf[1] = '\0';
            count += put_padded(buf, width, ' ');
            break;
        case 's': {
            const char* s = va_arg(ap, const char*);
            count += put_padded(s ? s : "(null)", width, ' ');
            break;
        }
        case 'd':
        case 'i': {
            int n = va_arg(ap, int);
            char* s = utoa(n < 0 ? -(uint32_t)n : (uint32_t)n, 10, 0, buf + sizeof(buf) - 1);
            if (n < 0) {
                if (pad == '0') {
                    terminal_putchar('-');
                    count++;
                    width--;
                } else {
                    *--s = '-';
                }
            }
            count += put_padded(s, width, pad);
            break;
        }
        case 'u':
            count += put_padded(utoa(va_arg(ap, uint32_t), 10, 0, buf + sizeof(buf) - 1), width, pad);
            break;
        case 'x':
        case 'X':
            count += put_padded(utoa(va_arg(ap, uint32_t), 16, *fmt == 'X', buf + sizeof(buf) - 1), width, pad);
            break;
        case 'p':
            terminal_write("0x");
            count += 2 + put_padded(utoa((uint32_t)va_arg(ap, void*), 16, 0, buf + sizeof(buf) - 1), 8, '0');
            break;
        case '%':
            terminal_putchar('%');
            count++;
            break;
        case '\0':
            fmt--;
            break;
        default:
            terminal_putchar('%');
            terminal_putchar(*fmt);
            count += 2;
            break;
        }
    }
    va_end(ap);
    return count;
}
