#include "../include/video/vga.h"
#include "../include/boot/multiboot.h"
#include "../include/shell/shell.h"
#include "../include/keyboard/kb.h"
#include "../include/kernel/rtc/rtc.h"
#include "../include/mm/mm.h"
#include "../include/kernel/panic/panic.h"
#include "../include/kernel/panic/boot.h"

extern uint32_t multiboot_info_ptr;
extern uint32_t __bitmap_start;

__attribute__((section(".multiboot"), aligned(4)))
const struct multiboot_header multiboot_header = {
    MULTIBOOT_HEADER_MAGIC,
    MULTIBOOT_HEADER_FLAGS,
    -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
};

void kernel_halt(void) {
    __asm__ volatile ("cli");
    while (1) {
        __asm__ volatile ("hlt");
    }
}

void delay(uint32_t milliseconds) {
    for (uint32_t i = 0; i < milliseconds * 1000; i++) {
        __asm__ volatile ("nop");
    }
}

void display_banner(void) {
    struct rtc_date current_date;
    rtc_read_full(&current_date);
    
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_puts(
        "  ____  _    _ _   _ _______   __\n"
        " |  _ \\| |  | | \\ | |_   _\\ \\ / /\n"
        " | |_) | |  | |  \\| | | |  \\ V / \n"
        " |  _ <| |  | | . ` | | |   > <  \n"
        " | |_) | |__| | |\\  |_| |_ / . \\ \n"
        " |____/ \\____/|_| \\_|_____/_/ \\_\\\n"
        "\n"
        "Welcome to Bunix - A Unix-Like Operating System\n"
        "Build Date: " __DATE__ " " __TIME__ "\n"
    );
    
    vga_puts("Copyright (C) 20");
    vga_putchar('0' + ((current_date.year % 100) / 10));
    vga_putchar('0' + (current_date.year % 10));
    vga_puts(" Bunix Team. All rights reserved.\n\n");
    vga_puts("Type 'help' for available commands\n");
    vga_puts("Project URL: https://github.com/0x16000/Bunix\n\n");
    
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
}

int main(void) {
    // Display boot screen
    boot_screen();
    
    // Display system banner
    display_banner();

    // NOW show the shell prompt
    print_shell_prompt();
    
    // Enable keyboard input now that everything is ready
    kb_enable_input(true);
    
    // Start interactive shell
    shell_run();
    
    // Should never reach here
    panic("Unexpected return from shell");
    return 0;
}
