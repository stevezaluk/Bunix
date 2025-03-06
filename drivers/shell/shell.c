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
    vga_puts("  echo <text> - Print the provided text\n");
    vga_puts("  cpuinfo - Display CPU architecture information\n");
}

// Command: clear
static void shell_clear(void) {
    vga_clear(); // Clear the screen (resets cursor to 0,0)
}

// Command: echo
static void shell_echo(const char *text) {
    vga_puts(text);
    vga_puts("\n");
}

// Function to detect CPU architecture
static void detect_cpu_architecture(void) {
    unsigned int eax, ebx, ecx, edx;
    char vendor[13];

    // Get CPU vendor ID
    __asm__ volatile("cpuid"
                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                     : "a"(0));
    *(unsigned int *)&vendor[0] = ebx;
    *(unsigned int *)&vendor[4] = edx;
    *(unsigned int *)&vendor[8] = ecx;
    vendor[12] = '\0';

    // Get CPU features
    __asm__ volatile("cpuid"
                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                     : "a"(1));

    // Check if the CPU supports long mode (x86_64)
    if (edx & (1 << 29)) {
        vga_puts("Architecture: x86_64\n");
    } else {
        vga_puts("Architecture: i386\n");
    }
}

// Command: cpuinfo
static void shell_cpuinfo(void) {
    detect_cpu_architecture();
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

            // Parse the input command
            if (strcmp(input, "help") == 0) {
                shell_help();
            } else if (strcmp(input, "clear") == 0) {
                shell_clear(); // Clears screen but does NOT print prompt
            } else if (strncmp(input, "echo ", 5) == 0) {
                // Extract the text after "echo "
                shell_echo(input + 5);
            } else if (strcmp(input, "cpuinfo") == 0) {
                shell_cpuinfo();
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
