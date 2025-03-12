#include "../include/video/vga.h"
#include "../include/boot/multiboot.h"
#include "../include/shell/shell.h"
#include "../include/keyboard/kb.h"
#include "../include/kernel/rtc/rtc.h"

// Function prototypes
int main(void);
void display_banner(void);
void kernel_halt(void);

// Multiboot header must be in the .multiboot section
__attribute__((section(".multiboot")))
const struct multiboot_header multiboot_header = {
    MULTIBOOT_HEADER_MAGIC,
    MULTIBOOT_HEADER_FLAGS,
    -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
};

// Entry point for the kernel
void _start(void) {
    main(); // Call the main function
    kernel_halt(); // Halt the CPU
}

// Function to display the ASCII art banner
void display_banner(void) {
    // Read the current date from the RTC
    struct rtc_date current_date;
    rtc_read_full(&current_date);

    // Display the ASCII art banner for "BUNIX"
    vga_puts("  ____  _    _ _   _ _______   __\n");
    vga_puts(" |  _ \\| |  | | \\ | |_   _\\ \\ / /\n");
    vga_puts(" | |_) | |  | |  \\| | | |  \\ V / \n");
    vga_puts(" |  _ <| |  | | . ` | | |   > <  \n");
    vga_puts(" | |_) | |__| | |\\  |_| |_ / . \\ \n");
    vga_puts(" |____/ \\____/|_| \\_|_____/_/ \\_\\\n");


    vga_puts("\n");
    vga_puts("Welcome to Bunix - A Unix-Like Operating System\n");
    vga_puts("Build Date: " __DATE__ " " __TIME__ "\n");

    // Display the copyright with the current year
    vga_puts("Copyright (C) 20"); // Start of the copyright line
    vga_putchar('0' + (current_date.year / 10)); // Tens digit of the year
    vga_putchar('0' + (current_date.year % 10)); // Units digit of the year
    vga_puts(" Bunix Team. All rights reserved.\n");

    vga_puts("\n");
    vga_puts("Type 'help' for a list of available commands.\n");
    vga_puts("For more information, visit: https://github.com/0x16000/Bunix\n");

    vga_puts("\n");
}

// Function to halt the CPU
void kernel_halt(void) {
    while (1) {
        __asm__ volatile ("hlt"); // Use the HLT instruction to halt the CPU
    }
}

int main(void) {
    // Initialize the VGA text mode
    if (vga_initialize() != 0) {  // Changed to check for non-zero return value
        // Handle VGA initialization failure
        kernel_halt();
    }

    // Display the ASCII art banner
    display_banner();

    // Initialize the keyboard driver
    if (kb_init() != 0) {  // Changed to check for non-zero return value
        // Handle keyboard initialization failure
        vga_puts("Keyboard initialization failed!\n");
        kernel_halt();
    }

    // Initialize the shell
    if (shell_init() != 0) {  // Changed to check for non-zero return value
        // Handle shell initialization failure
        vga_puts("Shell initialization failed!\n");
        kernel_halt();
    }

    // Run the shell
    shell_run();

    // The shell_run function should never return, but if it does, halt the CPU
    kernel_halt();

    return 0; // This return statement is unreachable
}
