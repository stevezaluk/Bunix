#ifndef KB_H
#define KB_H

#include <stdint.h>
#include <stdbool.h>

#define KB_DATA_PORT 0x60
#define KB_STATUS_PORT 0x64

typedef struct {
    bool shift_pressed;
    bool ctrl_pressed;
    bool alt_pressed;
    bool caps_lock;
    bool input_enabled;
    bool boot_complete;  // New flag to track boot state
} kb_state_t;

int kb_init(void);
void kb_enable_input(bool enable);
void kb_set_boot_complete(bool complete);  // New function
char kb_getchar(void);

#endif // KB_H
