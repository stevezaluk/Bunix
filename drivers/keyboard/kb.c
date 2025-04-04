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

// Shift-scancode to ASCII mapping
static const char kb_shift_scancode_to_ascii[128] = {
    0, 27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',
    '\t', 'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',
    0, 'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~', 0, '|',
    'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?', 0, '*', 0, ' ', 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, '-', 0, 0, 0, '+', 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

static kb_state_t kb_state = {
    false, false, false, false, 
    false, false  // input_enabled and boot_complete start false
};

// Initialize keyboard
int kb_init(void) {
    kb_state.shift_pressed = false;
    kb_state.ctrl_pressed = false;
    kb_state.alt_pressed = false;
    kb_state.caps_lock = false;
    kb_state.input_enabled = false;
    kb_state.boot_complete = false;
    return 0;
}

// Enable/disable input processing
void kb_enable_input(bool enable) {
    kb_state.input_enabled = enable;
}

// Mark boot completion
void kb_set_boot_complete(bool complete) {
    kb_state.boot_complete = complete;
}

// Convert scancode to ASCII
static char scancode_to_ascii(uint8_t scancode) {
    if (scancode >= 128) return 0;
    
    bool shift = kb_state.shift_pressed;
    bool caps_lock = kb_state.caps_lock;
    
    if (caps_lock && (kb_scancode_to_ascii[scancode] >= 'a') 
        && (kb_scancode_to_ascii[scancode] <= 'z')) {
        shift = !shift;
    }
    
    return shift ? kb_shift_scancode_to_ascii[scancode] 
                : kb_scancode_to_ascii[scancode];
}

// Handle key release
static void handle_key_release(uint8_t scancode) {
    if (!kb_state.boot_complete) return;
    
    switch (scancode) {
        case 0x2A: case 0x36: kb_state.shift_pressed = false; break;
        case 0x1D: kb_state.ctrl_pressed = false; break;
        case 0x38: kb_state.alt_pressed = false; break;
    }
}

// Handle key press
static void handle_key_press(uint8_t scancode) {
    if (!kb_state.boot_complete) return;
    
    switch (scancode) {
        case 0x2A: case 0x36: kb_state.shift_pressed = true; break;
        case 0x1D: kb_state.ctrl_pressed = true; break;
        case 0x38: kb_state.alt_pressed = true; break;
        case 0x3A: kb_state.caps_lock = !kb_state.caps_lock; break;
    }
}

// Get character from keyboard
char kb_getchar(void) {
    if (!kb_state.input_enabled) return 0;
    
    uint8_t scancode;
    while (1) {
        // Wait for key press
        while ((inb(KB_STATUS_PORT) & 0x01) == 0);
        
        scancode = inb(KB_DATA_PORT);
        
        // Skip releases
        if (scancode & 0x80) {
            handle_key_release(scancode & 0x7F);
            continue;
        }
        
        // Handle press
        handle_key_press(scancode);
        
        // Convert to ASCII
        char c = scancode_to_ascii(scancode);
        if (c != 0) return c;
    }
}
