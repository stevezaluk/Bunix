#include "../../include/video/vga.h"
#include "../../include/boot/multiboot.h"
#include "../../include/mm/mm.h"
#include "../../include/kernel/panic/panic.h"
#include "../../include/keyboard/kb.h"
#include "../../include/shell/shell.h"

// Declare multiboot_info_ptr as an external variable
extern uint32_t multiboot_info_ptr;
// Declare __bitmap_start from the linker script
extern uint32_t __bitmap_start;

// Simple delay function
static void delay(uint32_t milliseconds) {
    for (uint32_t i = 0; i < milliseconds * 1000; i++) {
        __asm__ volatile ("nop"); // No operation (delay)
    }
}

void boot_screen(void) {
    // Initialize VGA and check if it worked
    if (vga_initialize() != 0) {
        panic("Failed to initialize VGA in the booting process\n");
    }
    
    // Display VGA initialization message
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_puts("[ 0.000001] VGA initialized\n");
    
    // Delay for 1 second (for testing)
    delay(100000);
    
    // Display Keyboard initialization message (but don't enable input yet)
    if (kb_init() != 0) {
        panic("Failed to initialize Keyboard in the booting process\n");
    }
    vga_puts("[ 0.000002] Keyboard initialized\n");
    delay(100000);
    
    // Display Shell initialization
    vga_puts("[ 0.000003] Shell initialized\n");
    delay(400000);
    
    // Clear the screen and reset to default color
    vga_clear();
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}
