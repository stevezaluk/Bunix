#include "../include/video/vga.h"
#include "../include/kernel/ports/ports.h"
#include "../include/shell/shell.h"

static void delay(uint32_t count) {
    for(volatile uint32_t i = 0; i < count; i++);
}

void yes_command(const char *args) {
    const char *message = "y";
    uint8_t use_delay = 0;
    uint8_t color = VGA_COLOR_LIGHT_GREY;
    
    // Simple argument parsing
    if (args && args[0] != '\0') {
        message = args;
    }
    
    vga_set_color(color, VGA_COLOR_BLACK);
    
    while(1) {
        vga_puts(message);
        vga_putchar('\n');
        
        if(use_delay) {
            delay(1000000);
        }
        
        // Basic keyboard check
        if((inb(0x64) & 0x01)) {
            uint8_t scancode = inb(0x60);
            if(scancode == 0x81) { // CTRL+C
                vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
                vga_puts("\n[yes interrupted]\n");
                return;
            }
        }
    }
}
