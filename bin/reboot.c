#include "../include/shell/shell.h"
#include "../include/video/vga.h"
#include <stdint.h>

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

void reboot_command(const char *args) {
    vga_puts("Attempting system reboot...\n");

    // Try 8042 controller reset
    uint8_t status;
    int timeout = 100000;
    do {
        __asm__ volatile ("inb $0x64, %0" : "=a"(status));
        if (--timeout == 0) break;
    } while (status & 0x02);

    if (timeout > 0) {
        __asm__ volatile ("outb %0, $0x64" : : "a"((uint8_t)0xFE));
        for (;;) __asm__ volatile ("hlt");
    }

    vga_puts("Reboot failed, forcing reset...\n");
    system_fallback_reset();
}
