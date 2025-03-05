#ifndef KB_H
#define KB_H

#include <stdint.h>

// Keyboard I/O ports
#define KB_DATA_PORT 0x60
#define KB_STATUS_PORT 0x64

// Initialize the keyboard driver
void kb_init(void);

// Read a character from the keyboard
char kb_getchar(void);

#endif // KB_H
