#ifndef DEBUG_H
#define DEBUG_H

#include <stdint.h>
#include <stddef.h>
#include <stdarg.h>
#include "../../video/vga.h"

// Debug levels
#define DEBUG_LEVEL_NONE   0
#define DEBUG_LEVEL_ERROR 1
#define DEBUG_LEVEL_WARN  2
#define DEBUG_LEVEL_INFO  3
#define DEBUG_LEVEL_DEBUG 4
#define DEBUG_LEVEL_TRACE 5

// Debug colors
#define DEBUG_COLOR_SUCCESS VGA_COLOR_GREEN
#define DEBUG_COLOR_ERROR   VGA_COLOR_RED
#define DEBUG_COLOR_WARN    VGA_COLOR_YELLOW
#define DEBUG_COLOR_INFO    VGA_COLOR_LIGHT_BLUE
#define DEBUG_COLOR_DEBUG   VGA_COLOR_LIGHT_GREY
#define DEBUG_COLOR_TRACE   VGA_COLOR_DARK_GREY
#define DEBUG_COLOR_LABEL   VGA_COLOR_WHITE

// Debug macros
#define DEBUG_INIT() debug_initialize()
#define DEBUG_SUCCESS(fmt, ...) debug_print(DEBUG_LEVEL_INFO, DEBUG_COLOR_SUCCESS, " OK ", fmt, ##__VA_ARGS__)
#define DEBUG_ERROR(fmt, ...) debug_print(DEBUG_LEVEL_ERROR, DEBUG_COLOR_ERROR, " !! ", fmt, ##__VA_ARGS__)
#define DEBUG_WARN(fmt, ...) debug_print(DEBUG_LEVEL_WARN, DEBUG_COLOR_WARN, " ?? ", fmt, ##__VA_ARGS__)
#define DEBUG_INFO(fmt, ...) debug_print(DEBUG_LEVEL_INFO, DEBUG_COLOR_INFO, " -- ", fmt, ##__VA_ARGS__)
#define DEBUG_DEBUG(fmt, ...) debug_print(DEBUG_LEVEL_DEBUG, DEBUG_COLOR_DEBUG, " ** ", fmt, ##__VA_ARGS__)
#define DEBUG_TRACE(fmt, ...) debug_print(DEBUG_LEVEL_TRACE, DEBUG_COLOR_TRACE, " >> ", fmt, ##__VA_ARGS__)

// Function prototypes
void debug_initialize(void);
void debug_set_level(uint8_t level);
void debug_print(uint8_t level, uint8_t color, const char* prefix, const char* fmt, ...);
void debug_print_raw(const char* str);
void debug_print_hex(uint32_t num);
void debug_print_dec(uint32_t num, uint8_t digits);

#endif // DEBUG_H
