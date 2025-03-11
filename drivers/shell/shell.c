#include "../../include/shell/shell.h"
#include "../../include/video/vga.h"
#include "../../include/keyboard/kb.h"
#include "../../include/lib/string.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// Shell prompt
static const char *SHELL_PROMPT = "$ ";

// Command structure
typedef struct {
    const char *name;
    void (*func)(const char *);
} Command;

// Command declarations
void help_command(const char *args);
void clear_command(const char *args);
void echo_command(const char *args);
void cpuinfo_command(const char *args);
void reboot_command(const char *args);
void shutdown_command(const char *args);
void time_command(const char *args);
void date_command(const char *args);

// List of commands
static const Command commands[] = {
    {"help", help_command},
    {"clear", clear_command},
    {"echo", echo_command},
    {"cpuinfo", cpuinfo_command},
    {"reboot", reboot_command},
    {"shutdown", shutdown_command},
    {"time=", time_command},
    {"date", date_command},
    {NULL, NULL}
};

// Initialize the shell
int shell_init(void) {
    vga_puts(SHELL_PROMPT);
    return 0;
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
            bool command_found = false;
            for (const Command *cmd = commands; cmd->name != NULL; cmd++) {
                if (strncmp(input, cmd->name, strlen(cmd->name)) == 0) {
                    cmd->func(input + strlen(cmd->name));
                    command_found = true;
                    break;
                }
            }

            if (!command_found && input[0] != '\0') {
                vga_puts("Command not found: ");
                vga_puts(input);
                vga_puts("\n");
            }

            // Reset input buffer and reprint prompt
            index = 0;
            vga_puts(SHELL_PROMPT);
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
