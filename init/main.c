#include "../include/video/vga.h"
#include "../include/boot/multiboot.h"
#include "../include/shell/shell.h"
#include "../include/keyboard/kb.h"

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
    vga_puts("  ___ _   _ _  _ _____  __\n");
    vga_puts(" | _ ) | | | \\| |_ _\\ \\/ /\n");
    vga_puts(" | _ \\ |_| | .` || | >  <\n");
    vga_puts(" |___/\\___/|_|\\_|___/_/\\_\\\n");

    vga_puts("\n");
    vga_puts("All rights reserved.\n");

    vga_puts("\n");
    vga_puts("Type 'help' for available commands.\n");

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
