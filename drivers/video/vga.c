#include "../../include/video/vga.h"
#include "../../include/kernel/ports.h"
#include <stddef.h>

static uint16_t* const VGA_MEMORY = (uint16_t*) 0xB8000;

static size_t vga_row;
static size_t vga_column;
static uint8_t vga_color;
static uint16_t* vga_buffer;

// Initialize VGA
void vga_initialize(void) {
    vga_row = 0;
    vga_column = 0;
    vga_color = vga_entry_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_buffer = VGA_MEMORY;
    vga_clear();
    vga_enable_cursor(); // Enable the cursor
}

// Clear the screen
void vga_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            vga_buffer[index] = vga_entry(' ', vga_color);
        }
    }
    vga_row = 0; // Reset cursor row
    vga_column = 0; // Reset cursor column
    vga_update_cursor(vga_column, vga_row); // Update cursor position
}

// Print a character
void vga_putchar(char c) {
    if (c == '\n') {
        vga_column = 0;
        if (++vga_row == VGA_HEIGHT) {
            vga_row = 0;
        }
    } else if (c == '\b') { // Handle backspace
        if (vga_column > 0) {
            vga_column--;
        } else if (vga_row > 0) {
            vga_row--;
            vga_column = VGA_WIDTH - 1;
        }
        // Erase the character at the current position
        size_t index = vga_row * VGA_WIDTH + vga_column;
        vga_buffer[index] = vga_entry(' ', vga_color);
    } else {
        size_t index = vga_row * VGA_WIDTH + vga_column;
        vga_buffer[index] = vga_entry(c, vga_color);
        if (++vga_column == VGA_WIDTH) {
            vga_column = 0;
            if (++vga_row == VGA_HEIGHT) {
                vga_row = 0;
            }
        }
    }
    vga_update_cursor(vga_column, vga_row); // Update cursor position
}

// Print a string
void vga_puts(const char* str) {
    while (*str) {
        vga_putchar(*str++);
    }
}

// Enable the cursor 
void vga_enable_cursor(void) {
    outb(0x3D4, 0x0A); // Set cursor start register
    outb(0x3D5, 14);   // Cursor starts at scanline 14 (out of 0-15)

    outb(0x3D4, 0x0B); // Set cursor end register
    outb(0x3D5, 15);   // Cursor ends at scanline 15 (out of 0-15)
}

// Disable the cursor
void vga_disable_cursor(void) {
    outb(0x3D4, 0x0A); // Set cursor start register
    outb(0x3D5, 0x20); // Disable cursor (bit 5 set to 1)
}

// Update the cursor position
void vga_update_cursor(int x, int y) {
    uint16_t pos = y * VGA_WIDTH + x;

    outb(0x3D4, 0x0F); // Set low byte of cursor position
    outb(0x3D5, (uint8_t) (pos & 0xFF));

    outb(0x3D4, 0x0E); // Set high byte of cursor position
    outb(0x3D5, (uint8_t) ((pos >> 8) & 0xFF));
}
