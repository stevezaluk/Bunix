#include "../include/shell/shell.h"
#include "../include/video/vga.h"
#include "../include/lib/string.h"

void echo_command(const char *args) {
    if (args == NULL || *args == '\0') {
        vga_puts("echo: missing argument\nUsage: echo <text>\n");
        return;
    }
    
    // Skip leading whitespace
    while (*args == ' ') args++;
    
    vga_puts(args);
    vga_puts("\n");
}
