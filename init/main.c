#include "../include/video/vga.h"
#include "../include/boot/multiboot.h"
#include "../include/shell/shell.h"
#include "../include/keyboard/kb.h"

// Function prototype for main
int main(void);

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
    while (1); // Halt the CPU (infinite loop)
}

int main(void) {
    vga_initialize();

    // Print the ASCII art
    vga_puts("  ___ _   _ _  _ _____  __\n");
    vga_puts(" | _ ) | | | \\| |_ _\\ \\/ /\n");
    vga_puts(" | _ \\ |_| | .` || | >  <\n");
    vga_puts(" |___/\\___/|_|\\_|___/_/\\_\\\n");

    vga_puts("\n");
    vga_puts("All rights reserved.\n");

    vga_puts("\n");
    vga_puts("Type help for available commands.\n");

    vga_puts("\n");

    kb_init();      // Initialize the keyboard driver
    shell_init();   // Initialize the shell
    shell_run();    // Run the shell
    return 0;
}
