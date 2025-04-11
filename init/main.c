#include "../include/video/vga.h"
#include "../include/boot/multiboot.h"
#include "../include/shell/shell.h"
#include "../include/keyboard/kb.h"
#include "../include/kernel/rtc/rtc.h"
#include "../include/mm/vmm.h"
#include "../include/kernel/panic/panic.h"
#include "../include/kernel/panic/boot.h"
#include "../include/version/version.h"

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
    
    // Set a pleasant, eye-friendly color scheme
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_puts(   
    "______             _       \n"
    "| ___ \\           (_)      \n"
    "| |_/ /_   _ _ __  ___  __ \n"
    "| ___ \\ | | | '_ \\| | \\/ / \n"
    "| |_/ / |_| | | | | |>  <  \n"
    "\\____/ \\__,_|_| |_|_|_/\\_\\ \n"
    "                           \n"
    );
    // Subtle separator
    vga_set_color(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
    vga_puts("────────────────────────────────────────────\n");
    
    // System information with minimal color variation
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("Welcome to Bunix - A Minimal Unix-like OS\n\n");
    
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_puts("Version:    ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts(BUNIX_VERSION "\n");  // Changed this line
    
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_puts("Build:      ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts(__DATE__ " " __TIME__ "\n");
    
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_puts("Copyright:  ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("20");
    vga_putchar('0' + ((current_date.year % 100) / 10));
    vga_putchar('0' + (current_date.year % 10));
    vga_puts(" The Bunix Project\n");
    
    // Subtle separator
    vga_set_color(VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK);
    vga_puts("────────────────────────────────────────────\n");
    
    // Help prompt
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_puts("Type ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("'help'");
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_puts(" for available commands\n");
    
    // Project URL - slightly emphasized but still subtle
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_puts("Source:     ");
    vga_set_color(VGA_COLOR_WHITE, VGA_COLOR_BLACK);
    vga_puts("https://github.com/0x16000/Bunix\n\n");
    
    // Reset to default color
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
