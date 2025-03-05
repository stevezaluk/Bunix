#include "../../include/shell/shell.h"
#include "../../include/video/vga.h"
#include "../../include/keyboard/kb.h"
#include "../../include/lib/string.h"
#include <stdbool.h>

// Shell prompt
static const char *SHELL_PROMPT = "bunix> ";

// Command: help
static void shell_help(void) {
    vga_puts("Available commands:\n");
    vga_puts("  help - Show this help message\n");
    vga_puts("  clear - Clear the screen\n");
}

// Command: clear
static void shell_clear(void) {
    vga_clear(); // Clear the screen (resets cursor to 0,0)
}

// Initialize the shell
void shell_init(void) {
    vga_puts("Welcome to Bunix Shell!\n");
    vga_puts(SHELL_PROMPT);
}

// Run the shell
void shell_run(void) {
    char input[256];
    int index = 0;

    while (1) {
        // Read a character from the keyboard
        char c = kb_getchar();

        // Handle backspace
        if (c == '\b') {
            if (index > 0) {
                index--;
                vga_putchar('\b');
            }
        }
        // Handle enter
        else if (c == '\n') {
            input[index] = '\0';
            vga_puts("\n");

            // Execute the command
            if (strcmp(input, "help") == 0) {
                shell_help();
            } else if (strcmp(input, "clear") == 0) {
                shell_clear(); // Clears screen but does NOT print prompt
            } else if (input[0] != '\0') {
                vga_puts("Unknown command: ");
                vga_puts(input);
                vga_puts("\n");
            }

            // Reset input buffer and reprint prompt
            index = 0;
            vga_puts(SHELL_PROMPT); // Prompt is printed HERE, not in shell_clear
        }
        // Handle regular characters
        else {
            if (index < sizeof(input) - 1) {
                input[index++] = c;
                vga_putchar(c);
            }
        }
    }
}
