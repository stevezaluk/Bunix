// bin/uname.c - System information command
#include "../include/video/vga.h"
#include "../include/lib/string.h"

// Print usage information
static void print_usage() {
    vga_puts("Usage: uname [OPTION]...\n");
    vga_puts("Print system information.\n\n");
    vga_puts("Options:\n");
    vga_puts("  -r     Print kernel release\n");
    vga_puts("  --help Display this help\n");
}

// The command function that will be called from shell.c
void uname_command(const char *args) {
    if (args == NULL || args[0] == '\0') {
        // Basic case: just "uname"
        vga_puts("Bunix\n");
    }
    else {
        // Tokenize arguments (simple version)
        char *arg = strtok((char *)args, " ");
        
        if (arg[0] == '-') {
            if (strcmp(arg, "-r") == 0) {
                vga_puts("0.1-bunix\n");
            }
            else if (strcmp(arg, "--help") == 0) {
                print_usage();
            }
            else {
                // Invalid option
                vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
                vga_puts("uname: invalid option '");
                vga_puts(arg);
                vga_puts("'\n");
                print_usage();
            }
        }
        else {
            // Argument doesn't start with -
            vga_set_color(VGA_COLOR_LIGHT_RED, VGA_COLOR_BLACK);
            vga_puts("uname: unrecognized argument '");
            vga_puts(arg);
            vga_puts("'\n");
            print_usage();
        }
    }
}
