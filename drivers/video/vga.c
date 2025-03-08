#include "../../include/video/vga.h"
#include "../../include/kernel/ports/ports.h"
#include <stddef.h>

// VGA memory address
static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

// Screen dimensions
#define VGA_WIDTH  80
#define VGA_HEIGHT 25

// Cursor control ports
#define VGA_CRTC_ADDR 0x3D4
#define VGA_CRTC_DATA 0x3D5

// Cursor control registers
#define VGA_CURSOR_START_REG 0x0A
#define VGA_CURSOR_END_REG   0x0B
#define VGA_CURSOR_HIGH_REG  0x0E
#define VGA_CURSOR_LOW_REG   0x0F

// Cursor disable value
#define VGA_CURSOR_DISABLE 0x20

// Current cursor position and color
static size_t vga_row;
static size_t vga_column;
static uint8_t vga_color;
static uint16_t* vga_buffer;

// Scroll the screen up by one line
static void vga_scroll(void) {
    for (size_t y = 1; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index_current = y * VGA_WIDTH + x;
            const size_t index_above = (y - 1) * VGA_WIDTH + x;
            vga_buffer[index_above] = vga_buffer[index_current];
        }
    }

    // Clear the bottom line
    for (size_t x = 0; x < VGA_WIDTH; x++) {
        const size_t index = (VGA_HEIGHT - 1) * VGA_WIDTH + x;
        vga_buffer[index] = vga_entry(' ', vga_color);
    }

    vga_row = VGA_HEIGHT - 1;
    vga_column = 0;
}

// Initialize VGA
void vga_initialize(void) {
    vga_row = 0;
    vga_column = 0;
    vga_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_buffer = VGA_MEMORY;
    vga_clear();
    vga_enable_cursor();
}

// Clear the screen
void vga_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            vga_buffer[index] = vga_entry(' ', vga_color);
        }
    }
    vga_row = 0;
    vga_column = 0;
    vga_update_cursor(vga_column, vga_row);
}

// Print a character
void vga_putchar(char c) {
    if (c == '\n') {
        vga_column = 0;
        if (++vga_row == VGA_HEIGHT) {
            vga_scroll();
        }
    } else if (c == '\b') {
        if (vga_column > 0) {
            vga_column--;
        } else if (vga_row > 0) {
            vga_row--;
            vga_column = VGA_WIDTH - 1;
        }
        size_t index = vga_row * VGA_WIDTH + vga_column;
        vga_buffer[index] = vga_entry(' ', vga_color);
    } else {
        size_t index = vga_row * VGA_WIDTH + vga_column;
        vga_buffer[index] = vga_entry(c, vga_color);
        if (++vga_column == VGA_WIDTH) {
            vga_column = 0;
            if (++vga_row == VGA_HEIGHT) {
                vga_scroll();
            }
        }
    }
    vga_update_cursor(vga_column, vga_row);
}

// Print a string
void vga_puts(const char* str) {
    while (*str) {
        vga_putchar(*str++);
    }
}

// Enable the cursor
void vga_enable_cursor(void) {
    outb(VGA_CRTC_ADDR, VGA_CURSOR_START_REG);
    outb(VGA_CRTC_DATA, 14); // Cursor starts at scanline 14

    outb(VGA_CRTC_ADDR, VGA_CURSOR_END_REG);
    outb(VGA_CRTC_DATA, 15); // Cursor ends at scanline 15
}

// Disable the cursor
void vga_disable_cursor(void) {
    outb(VGA_CRTC_ADDR, VGA_CURSOR_START_REG);
    outb(VGA_CRTC_DATA, VGA_CURSOR_DISABLE);
}

// Update the cursor position
void vga_update_cursor(int x, int y) {
    uint16_t pos = y * VGA_WIDTH + x;

    outb(VGA_CRTC_ADDR, VGA_CURSOR_LOW_REG);
    outb(VGA_CRTC_DATA, (uint8_t) (pos & 0xFF));

    outb(VGA_CRTC_ADDR, VGA_CURSOR_HIGH_REG);
    outb(VGA_CRTC_DATA, (uint8_t) ((pos >> 8) & 0xFF));
}
