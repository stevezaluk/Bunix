#include "../../include/kernel/panic/debug.h"
#include "../../include/video/vga.h"
#include "../../include/kernel/ports/ports.h"
#include <stdarg.h>

static uint8_t debug_level = DEBUG_LEVEL_INFO;

// Forward declarations for helper functions
static size_t debug_format_dec(char* buf, int num);
static size_t debug_format_hex(char* buf, unsigned int num);
static size_t debug_format_str(char* buf, const char* str);

void debug_initialize(void) {
    vga_initialize();
    debug_level = DEBUG_LEVEL_INFO;
    DEBUG_INFO("Debug system initialized");
}

void debug_set_level(uint8_t level) {
    debug_level = level;
    DEBUG_INFO("Debug level set to %d", level);
}

void debug_print(uint8_t level, uint8_t color, const char* prefix, const char* fmt, ...) {
    if (level > debug_level) return;

    // Save current color
    uint8_t old_color = vga_get_color();

    // Print prefix with brackets and color
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_puts("[");
    
    vga_set_color(color, VGA_COLOR_BLACK);
    vga_puts(prefix);
    
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_puts("] ");
    
    // Restore label color for message
    vga_set_color(DEBUG_COLOR_LABEL, VGA_COLOR_BLACK);

    // Process variable arguments
    va_list args;
    va_start(args, fmt);
    
    // Temporary buffer for formatted output
    char buffer[256];
    char* ptr = buffer;
    
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            switch (*fmt) {
                case 'd': {
                    int num = va_arg(args, int);
                    ptr += debug_format_dec(ptr, num);
                    break;
                }
                case 'x': {
                    unsigned int num = va_arg(args, unsigned int);
                    ptr += debug_format_hex(ptr, num);
                    break;
                }
                case 's': {
                    char* str = va_arg(args, char*);
                    ptr += debug_format_str(ptr, str);
                    break;
                }
                case 'c': {
                    char c = (char)va_arg(args, int);
                    *ptr++ = c;
                    break;
                }
                default:
                    *ptr++ = *fmt;
                    break;
            }
        } else {
            *ptr++ = *fmt;
        }
        fmt++;
    }
    *ptr = '\0';
    
    va_end(args);
    
    // Print the formatted string
    vga_puts(buffer);
    vga_puts("\n");
    
    // Restore original color
    vga_set_color(old_color >> 4, old_color & 0x0F);
}

void debug_print_raw(const char* str) {
    vga_puts(str);
}

void debug_print_hex(uint32_t num) {
    vga_puthex(num);
}

void debug_print_dec(uint32_t num, uint8_t digits) {
    vga_putdec(num, digits);
}

/* Helper function implementations */

static size_t debug_format_dec(char* buf, int num) {
    if (num == 0) {
        *buf = '0';
        return 1;
    }

    char temp[16];
    char* ptr = temp;
    size_t len = 0;
    int is_neg = 0;

    if (num < 0) {
        is_neg = 1;
        num = -num;
    }

    while (num > 0) {
        *ptr++ = '0' + (num % 10);
        num /= 10;
        len++;
    }

    if (is_neg) {
        *buf++ = '-';
        len++;
    }

    while (ptr > temp) {
        *buf++ = *--ptr;
    }

    return len;
}

static size_t debug_format_hex(char* buf, unsigned int num) {
    const char hex_chars[] = "0123456789ABCDEF";
    size_t len = 0;

    *buf++ = '0';
    *buf++ = 'x';
    len += 2;

    int shift = 28;
    while (shift >= 0) {
        uint8_t nibble = (num >> shift) & 0xF;
        *buf++ = hex_chars[nibble];
        len++;
        shift -= 4;
    }

    return len;
}

static size_t debug_format_str(char* buf, const char* str) {
    size_t len = 0;
    while (*str) {
        *buf++ = *str++;
        len++;
    }
    return len;
}
