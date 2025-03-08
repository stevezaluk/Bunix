#include "../../include/kernel/ports/ports.h"
#include "../../include/keyboard/kb.h"
#include "../../include/video/vga.h"
#include <stdbool.h>

// Scancode to ASCII mapping (for US QWERTY keyboard)
static const char kb_scancode_to_ascii[128] = {
    0, 27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',
    '\t', 'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',
    0, 'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`', 0, '\\',
    'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/', 0, '*', 0, ' ', 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Shift-scancode to ASCII mapping (for uppercase letters and symbols)
static const char kb_shift_scancode_to_ascii[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Track the state of the shift key
static bool shift_pressed = false;

// Initialize the keyboard driver
void kb_init(void) {
    // No initialization needed for now
}

// Read a character from the keyboard
char kb_getchar(void) {
    char c = 0;
    uint8_t scancode;
    uint8_t status;

    do {
        // Wait until a key is pressed
        do {
            status = inb(KB_STATUS_PORT);
        } while ((status & 0x01) == 0);

        // Read the scancode
        scancode = inb(KB_DATA_PORT);

        // Handle key release events
        if (scancode & 0x80) {
            uint8_t released_scancode = scancode & 0x7F;
            if (released_scancode == 0x2A || released_scancode == 0x36) {
                shift_pressed = false; // Shift key released
            }
            continue;
        }

        // Handle shift key press
        if (scancode == 0x2A || scancode == 0x36) {
            shift_pressed = true; // Shift key pressed
            continue;
        }

        // Convert the scancode to ASCII
        if (scancode < 128) {
            c = shift_pressed ? kb_shift_scancode_to_ascii[scancode] : kb_scancode_to_ascii[scancode];
        }

    } while (c == 0);

    return c;
}
