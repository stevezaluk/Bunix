/**
 * Bunix Kernel Main Entry Point
 * 
 * This file contains the kernel initialization and main loop.
 */

#include "../include/video/vga.h"
#include "../include/boot/multiboot.h"
#include "../include/shell/shell.h"
#include "../include/keyboard/kb.h"
#include "../include/kernel/rtc/rtc.h"
#include "../include/mm/vmm.h"
#include "../include/kernel/panic/panic.h"
#include "../include/kernel/panic/boot.h"
#include "../include/version/version.h"

/* Multiboot header must be in the first 8KB of the kernel */
__attribute__((section(".multiboot"), aligned(4)))
static const struct multiboot_header multiboot_header = {
    .magic   = MULTIBOOT_HEADER_MAGIC,
    .flags   = MULTIBOOT_HEADER_FLAGS,
    .checksum = -(MULTIBOOT_HEADER_MAGIC + MULTIBOOT_HEADER_FLAGS)
};

/* External declarations */
extern uint32_t multiboot_info_ptr;
extern uint32_t __bitmap_start;

/**
 * kernel_halt - Halts the system indefinitely
 * 
 * Disables interrupts and enters an infinite halt loop.
 */
static void kernel_halt(void) 
{
    __asm__ volatile ("cli");
    for (;;) {
        __asm__ volatile ("hlt");
    }
}

/**
 * delay - Simple busy-wait delay
 * @milliseconds: Time to delay in milliseconds
 * 
 * Note: This is a very crude delay mechanism and should be
 * replaced with proper timer-based delays when available.
 */
static void delay(uint32_t milliseconds) 
{
    for (uint32_t i = 0; i < milliseconds * 1000; i++) {
        __asm__ volatile ("nop");
    }
}

/**
 * display_banner - Shows the system welcome banner
 * 
 * Displays version information, copyright notice, and
 * basic help instructions in a clean, formatted layout.
 */
static void display_banner(void) 
{
    struct rtc_date current_date;
    rtc_read_full(&current_date);
    
    /* Your ASCII art in a simple box */
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_puts(
        "\n"
        "+-------------------------------------+\n"
        "| ______             _               |\n"
        "| | ___ \\           (_)              |\n"
        "| | |_/ /_   _ _ __  ___  __         |\n"
        "| | ___ \\ | | | '_ \\| \\ \\/ /         |\n"
        "| | |_/ / |_| | | | | |>  <          |\n"
        "| \\____/ \\__,_|_| |_|_/_/\\_\\         |\n"
        "+-------------------------------------+\n"
        "\n"
    );
    
    /* System information - all light grey */
    vga_puts("  Version:    ");
    vga_puts(BUNIX_VERSION "\n");
    
    vga_puts("  Build Date: ");
    vga_puts(__DATE__ " " __TIME__ "\n");
    
    vga_puts("  Copyright:  20");
    vga_putchar('0' + ((current_date.year % 100) / 10));
    vga_putchar('0' + (current_date.year % 10));
    vga_puts(" Bunix OS\n");
    
    /* Simple separator */
    vga_puts("\n  ---------------------------------\n\n");
    
    /* Help prompt */
    vga_puts("  Type 'help' for available commands\n\n");
    
    /* Source info */
    vga_puts("  github.com/0x16000/Bunix\n\n");
}

/**
 * kernel_main - Main kernel entry point after boot
 * 
 * Initializes system components and starts the shell.
 */
int main(void) 
{
    /* Show boot animation */
    boot_screen();
    
    /* Display system information */
    display_banner();
    
    /* Initialize shell interface */
    print_shell_prompt();
    kb_enable_input(true);
    
    /* Enter main shell loop */
    shell_run();
    
    /* If we get here, something went wrong */
    panic("Kernel shell terminated unexpectedly");
    
    return 0; /* Never reached */
}
