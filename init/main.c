#include "../include/video/vga.h"
#include "../include/boot/multiboot.h"
#include "../include/shell/shell.h"
#include "../include/keyboard/kb.h"
#include "../include/kernel/rtc/rtc.h"
#include "../include/mm/mm.h"

// Declare multiboot_info_ptr as an external variable
extern uint32_t multiboot_info_ptr;

// Declare __bitmap_start from the linker script
extern uint32_t __bitmap_start;

// Function prototypes
int main(void);
void display_banner(void);
void kernel_halt(void);
void panic(const char *message);
void boot_screen(void);
void delay(uint32_t milliseconds);

// Multiboot header must be in the .multiboot section
__attribute__((section(".multiboot"), aligned(4)))
const struct multiboot_header multiboot_header = {
    MULTIBOOT_HEADER_MAGIC,
    MULTIBOOT_HEADER_FLAGS,
    -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
};

// Kernel panic handler
void panic(const char *message) {
    vga_puts("\nKERNEL PANIC: ");
    vga_puts(message);
    vga_puts("\nSystem halted!\n");
    kernel_halt();
}

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

// Boot screen display
void boot_screen(void) {
    vga_clear();
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK); // Set text color to green for boot screen

    // Display memory information
    struct multiboot_info* mb_info = (struct multiboot_info*)multiboot_info_ptr;
    if (mb_info->flags & MULTIBOOT_INFO_MEMORY) {
        uint64_t total_memory_bytes = (mb_info->mem_lower + mb_info->mem_upper) * 1024;
        vga_puts("Total Memory: ");
        vga_putdec(total_memory_bytes / (1024 * 1024), 0); // Convert bytes to MB
        vga_puts(" MB\n");

        vga_puts("Memory Below 1MB: ");
        vga_putdec(mb_info->mem_lower, 0);
        vga_puts(" KB\n");

        vga_puts("Memory Above 1MB: ");
        vga_putdec(mb_info->mem_upper, 0);
        vga_puts(" KB\n");
    } else {
        vga_puts("Memory information not available!\n");
    }

    // Display additional system information
    vga_puts("\nInitializing memory manager...\n");
    mm_init(&__bitmap_start, (mb_info->mem_lower + mb_info->mem_upper) * 1024);

    vga_puts("Total Frames: ");
    vga_putdec(mm_get_total_frames(), 0);
    vga_puts("\n");

    vga_puts("Free Frames: ");
    vga_putdec(mm_get_free_frames(), 0);
    vga_puts("\n");

    // Delay for 5 seconds (for testing)
    delay(1000000);

    // Clear the screen and reset to default color
    vga_clear();
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK); // Reset to default color
}

// Display system banner
void display_banner(void) {
    struct rtc_date current_date;
    rtc_read_full(&current_date); // No error checking since it's void

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

    // Reset to default text color (light grey on black)
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

// Main kernel initialization
int main(void) {
    // Initialize critical subsystems
    if (vga_initialize() != 0) {
        // If VGA fails, we can't display anything - just halt
        kernel_halt();
    }

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

    // Start interactive shell
    shell_run();

    // Should never reach here
    panic("Unexpected return from shell");
    return 0;
}
