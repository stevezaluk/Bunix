#include "../include/video/vga.h"
#include "../include/boot/multiboot.h"
#include "../include/shell/shell.h"
#include "../include/keyboard/kb.h"
#include "../include/kernel/rtc/rtc.h"
#include "../include/mm/mm.h"
#include "../include/kernel/panic/panic.h"
#include "../include/kernel/panic/boot.h"

// Declare multiboot_info_ptr as an external variable
extern uint32_t multiboot_info_ptr;
// Declare __bitmap_start from the linker script
extern uint32_t __bitmap_start;

// Function prototypes
int main(void);
void display_banner(void);
void kernel_halt(void);
void delay(uint32_t milliseconds);

// Multiboot header must be in the .multiboot section
__attribute__((section(".multiboot"), aligned(4)))
const struct multiboot_header multiboot_header = {
    MULTIBOOT_HEADER_MAGIC,
    MULTIBOOT_HEADER_FLAGS,
    -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
};

// Halt the CPU permanently
void kernel_halt(void) {
    __asm__ volatile ("cli"); // Disable interrupts
    while (1) {
        __asm__ volatile ("hlt"); // Halt CPU
    }
}

// Simple delay function
void delay(uint32_t milliseconds) {
    for (uint32_t i = 0; i < milliseconds * 1000; i++) {
        __asm__ volatile ("nop"); // No operation (delay)
    }
}

// Display system banner
void display_banner(void) {
    struct rtc_date current_date;
    rtc_read_full(&current_date);
    // Set color for the banner (light grey on black)
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_puts(
        "  ____  _    _ _   _ _______   __\n"
        " |  _ \\| |  | | \\ | |_   _\\ \\ / /\n"
        " | |_) | |  | |  \\| | | |  \\ V / \n"
        " |  _ <| |  | | . ` | | |   > <  \n"
        " | |_) | |__| | |\\  |_| |_ / . \\ \n"
        " |____/ \\____/|_| \\_|_____/_/ \\_\\\n"
        "\n"
        "Welcome to Bunix - An Unix-Like Operating System\n"
        "Build Date: " __DATE__ " " __TIME__ "\n"
    );
    // Display copyright with 4-digit year
    vga_puts("Copyright (C) 20");
    vga_putchar('0' + ((current_date.year % 100) / 10));
    vga_putchar('0' + (current_date.year % 10));
    vga_puts(" Bunix Team. All rights reserved.\n\n");
    vga_puts("Type 'help' for available commands\n");
    vga_puts("Project URL: https://github.com/0x16000/Bunix\n\n");
    // Reset to default text color
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

// Main kernel initialization
int main(void) {
    // Display boot screen
    boot_screen();
    
    // Display system banner
    display_banner();
    
    // Initialize hardware components
    if (kb_init() != 0) {
        panic("Keyboard initialization failed");
    }
    
    // Initialize system services
    if (shell_init() != 0) {
        panic("Shell initialization failed");
    }
    
    // NOW enable keyboard input
    kb_enable_input(true);
    
    // Start interactive shell
    shell_run();
    
    // Should never reach here
    panic("Unexpected return from shell");
    return 0;
}
