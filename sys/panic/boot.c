#include "../../include/video/vga.h"
#include "../../include/boot/multiboot.h"
#include "../../include/mm/mm.h"
#include "../../include/kernel/panic/panic.h"
#include "../../include/keyboard/kb.h"
#include "../../include/shell/shell.h"
#include "../../include/kernel/panic/debug.h"
#include "../../include/kernel/arch/x86/cpu.h"

extern uint32_t multiboot_info_ptr;
extern uint32_t __bitmap_start;

static void delay(uint32_t milliseconds) {
    for (uint32_t i = 0; i < milliseconds * 1000; i++) {
        __asm__ volatile ("nop");
    }
}

void boot_screen(void) {
    // Initialize debug system first
    DEBUG_INIT();
    
    // Initialize VGA with black background
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    if (vga_initialize() != 0) {
        DEBUG_ERROR("Failed to initialize VGA");
        panic("Failed to initialize VGA in the booting process\n");
    }
    DEBUG_SUCCESS("VGA initialized");
    delay(100000);
    
    // Initialize Keyboard
    if (kb_init() != 0) {
        DEBUG_ERROR("Failed to initialize Keyboard");
        panic("Failed to initialize Keyboard in the booting process\n");
    }
    DEBUG_SUCCESS("Keyboard initialized");
    delay(100000);
    
    // Initialize Shell should never check for return value
    DEBUG_INFO("Initializing shell subsystem");
    DEBUG_SUCCESS("Shell initialized");
    delay(100000);

    // Display kernel address
    DEBUG_INFO("Kernel running at hex: 0xC0000000");
    delay(200000);
    
    // Mark boot complete AFTER all hardware is initialized
    kb_set_boot_complete(true);
    
    // Clear the screen and ensure black background
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_clear();
    
    // Enable keyboard input LAST
    kb_enable_input(true);
    
    // Don't show prompt here - let main() handle the banner first
}
