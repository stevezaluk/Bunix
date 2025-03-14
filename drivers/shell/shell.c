#include "../../include/shell/shell.h"
#include "../../include/video/vga.h"
#include "../../include/keyboard/kb.h"
#include "../../include/lib/string.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

// Shell prompt
static const char *SHELL_PROMPT = "$ ";

// List of commands
Command commands[] = {
    {"help", help_command, "Show this help message"},
    {"clear", clear_command, "Clear the screen"},
    {"echo", echo_command, "Print text to the screen"},
    {"cpuinfo", cpuinfo_command, "Display CPU information"},
    {"reboot", reboot_command, "Reboot the system"},
    {"shutdown", shutdown_command, "Shutdown the system"},
    {"time", time_command, "Show the current time"},
    {"date", date_command, "Show the current date"},
    {"uptime", uptime_command, "Show system uptime"},
    {NULL, NULL, NULL} // End of array
};

// Command history
static char history[MAX_HISTORY_SIZE][256];
static int history_index = 0;

// Add a command to history
void add_to_history(const char *input) {
    strncpy(history[history_index], input, sizeof(history[history_index]) - 1);
    history_index = (history_index + 1) % MAX_HISTORY_SIZE;
}

// Initialize the shell
int shell_init(void) {
    vga_puts(SHELL_PROMPT);
    uptime_init();  // Initialize uptime
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

            // Add command to history
            add_to_history(input);

            // Parse the input command
            char *command_name = strtok(input, " "); // Extract the command name
            char *args = strtok(NULL, "");           // Extract the rest of the input as arguments

            if (command_name == NULL) {
                // Empty input, just reprint the prompt
                vga_puts(SHELL_PROMPT);
                continue;
            }

            // Look up the command in the command table
            bool command_found = false;
            for (const Command *cmd = commands; cmd->name != NULL; cmd++) {
                if (strcmp(command_name, cmd->name) == 0) {
                    cmd->func(args); // Pass the arguments to the command function
                    command_found = true;
                    break;
                }
            }

            if (!command_found) {
                vga_puts("Command not found: ");
                vga_puts(command_name);
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
