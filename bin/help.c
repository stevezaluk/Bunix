#include "../include/shell/shell.h"
#include "../include/video/vga.h"

void help_command(const char *args) {
    vga_puts("Available commands:\n");
    vga_puts("  help      - Show this help message\n");
    vga_puts("  clear     - Clear the screen\n");
    vga_puts("  echo <txt> - Print text\n");
    vga_puts("  cpuinfo   - Show CPU details\n");
    vga_puts("  reboot    - Reboot system\n");
    vga_puts("  shutdown  - Shutdown system\n");
    vga_puts("  time <zone> - Show time (de=Germany, us=US)\n");
    vga_puts("  date      - Show current date\n");
}
