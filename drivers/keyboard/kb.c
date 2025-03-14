#include "../../include/kernel/ports/ports.h"
#include "../../include/keyboard/kb.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

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

// Keyboard state structure
static kb_state_t kb_state = {false, false, false, false};

// Initialize the keyboard driver
int kb_init(void) {
    // Reset keyboard state
    kb_state.shift_pressed = false;
    kb_state.ctrl_pressed = false;
    kb_state.alt_pressed = false;
    kb_state.caps_lock = false;
    return 0;
}

// Handle key release events
static void handle_key_release(uint8_t scancode) {
    switch (scancode) {
        case 0x2A: // Left Shift
        case 0x36: // Right Shift
            kb_state.shift_pressed = false;
            break;
        case 0x1D: // Ctrl
            kb_state.ctrl_pressed = false;
            break;
        case 0x38: // Alt
            kb_state.alt_pressed = false;
            break;
    }
}

// Handle key press events
static void handle_key_press(uint8_t scancode) {
    switch (scancode) {
        case 0x2A: // Left Shift
        case 0x36: // Right Shift
            kb_state.shift_pressed = true;
            break;
        case 0x1D: // Ctrl
            kb_state.ctrl_pressed = true;
            break;
        case 0x38: // Alt
            kb_state.alt_pressed = true;
            break;
        case 0x3A: // Caps Lock
            kb_state.caps_lock = !kb_state.caps_lock; // Toggle Caps Lock state
            break;
    }
}

// Convert scancode to ASCII
static char scancode_to_ascii(uint8_t scancode) {
    if (scancode >= 128) return 0;

    bool shift = kb_state.shift_pressed;
    bool caps_lock = kb_state.caps_lock;

    // Determine the effective case
    if (caps_lock) {
        // Caps Lock is on: invert the shift state for letters only
        if (kb_scancode_to_ascii[scancode] >= 'a' && kb_scancode_to_ascii[scancode] <= 'z') {
            shift = !shift;
        }
    }

    return shift ? kb_shift_scancode_to_ascii[scancode] : kb_scancode_to_ascii[scancode];
}

// Read a character from the keyboard
char kb_getchar(void) {
    char c = 0;
    uint8_t scancode;

    while (1) {
        // Wait until a key is pressed
        while ((inb(KB_STATUS_PORT) & 0x01) == 0);

        // Read the scancode
        scancode = inb(KB_DATA_PORT);

        // Handle key release events
        if (scancode & 0x80) {
            handle_key_release(scancode & 0x7F);
            continue;
        }

        // Handle key press events
        handle_key_press(scancode);

        // Convert the scancode to ASCII
        c = scancode_to_ascii(scancode);

        if (c != 0) break;
    }

    return c;
}
