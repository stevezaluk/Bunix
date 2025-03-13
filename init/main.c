#include "../include/video/vga.h"
#include "../include/boot/multiboot.h"
#include "../include/shell/shell.h"
#include "../include/keyboard/kb.h"
#include "../include/kernel/rtc/rtc.h"

// Function prototypes
int main(void);
void display_banner(void);
void kernel_halt(void);
void panic(const char *message);

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

// Display system banner
void display_banner(void) {
    struct rtc_date current_date;
    rtc_read_full(&current_date); // No error checking since it's void

    vga_puts(
        "  ____  _    _ _   _ _______   __\n"
        " |  _ \\| |  | | \\ | |_   _\\ \\ / /\n"
        " | |_) | |  | |  \\| | | |  \\ V / \n"
        " |  _ <| |  | | . ` | | |   > <  \n"
        " | |_) | |__| | |\\  |_| |_ / . \\ \n"
        " |____/ \\____/|_| \\_|_____/_/ \\_\\\n"
        "\n"
        "Welcome to Bunix - A Unix-Like Operating System\n"
        "Build Date: " __DATE__ " " __TIME__ "\n"
    );

    // Display copyright with 4-digit year
    vga_puts("Copyright (C) 20");
    vga_putchar('0' + ((current_date.year % 100) / 10));
    vga_putchar('0' + (current_date.year % 10));
    vga_puts(" Bunix Team. All rights reserved.\n\n");

    vga_puts("Type 'help' for available commands\n");
    vga_puts("Project URL: https://github.com/0x16000/Bunix\n\n");
}

// Entry point called by bootloader
void _start(void) {
    main();
    kernel_halt();
}

// Main kernel initialization
int main(void) {
    // Initialize critical subsystems
    if (vga_initialize() != 0) {
        // If VGA fails, we can't display anything - just halt
        kernel_halt();
    }

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
