#include "../include/shell/shell.h"
#include "../include/video/vga.h"
#include <stdint.h>

// Function to force a system reset by causing a triple fault
static void system_fallback_reset() {
    // Create a null IDT descriptor
    uint8_t idt_descriptor[6] = {0};

    // Load the null IDT descriptor
    __asm__ volatile (
        "lidt (%0)"
        :
        : "r" (idt_descriptor)
        : "memory"
    );

    // Trigger a triple fault by dividing by zero
    __asm__ volatile ("int $0");
}

// Function to attempt a system reboot
void reboot_command(const char *args) {
    vga_puts("Attempting system reboot...\n");

    // Try 8042 controller reset
    uint8_t status;
    int timeout = 100000; // Timeout to prevent infinite loop

    // Wait for the 8042 controller to be ready
    do {
        __asm__ volatile ("inb $0x64, %0" : "=a"(status));
        if (--timeout == 0) {
            vga_puts("Timeout: 8042 controller not ready.\n");
            break;
        }
    } while (status & 0x02);

    // If the controller is ready, send the reset command
    if (timeout > 0) {
        __asm__ volatile ("outb %0, $0x64" : : "a"((uint8_t)0xFE));
        vga_puts("Reset command sent to 8042 controller.\n");

        // Halt the CPU indefinitely
        for (;;) {
            __asm__ volatile ("hlt");
        }
    } else {
        vga_puts("Reboot failed, forcing reset...\n");
        system_fallback_reset();
    }
}
