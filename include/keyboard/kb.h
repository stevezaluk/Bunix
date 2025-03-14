#ifndef KB_H
#define KB_H

#include <stdint.h>
#include <stdbool.h>

// Keyboard I/O ports
#define KB_DATA_PORT 0x60
#define KB_STATUS_PORT 0x64

// Keyboard state structure
typedef struct {
    bool shift_pressed;
    bool ctrl_pressed;
    bool alt_pressed;
    bool caps_lock;
} kb_state_t;

// Initialize the keyboard driver
int kb_init(void);

// Read a character from the keyboard
char kb_getchar(void);

#endif // KB_H
