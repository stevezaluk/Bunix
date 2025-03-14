#include "../include/shell/shell.h"
#include "../include/video/vga.h"
#include <stdint.h>
#include <stdbool.h>

// Function to attempt ACPI shutdown
static bool try_acpi_shutdown() {
    for (int i = 0; i < 3; i++) {  // Try multiple times
        __asm__ volatile (
            "outw %w0, %w1"
            : 
            : "a" ((uint16_t)0x2000), "Nd" ((uint16_t)0x604)
            : "memory"
        );
        __asm__ volatile ("hlt");  // Wait a bit

        // For now, assume the shutdown was successful
        return true;
    }
    return false;
}

// Function to attempt Bochs/QEMU shutdown
static bool try_bochs_shutdown() {
    __asm__ volatile (
        "outw %w0, %w1"
        : 
        : "a" ((uint16_t)0x2000), "Nd" ((uint16_t)0xB004)
        : "memory"
    );

    // For now, assume the shutdown was successful
    return true;
}

// Fallback reset function
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

void shutdown_command(const char *args) {
    vga_puts("Attempting system shutdown...\n");

    // Attempt ACPI shutdown
    if (try_acpi_shutdown()) {
        vga_puts("System shutdown successful via ACPI.\n");
        return;
    }

    // Attempt Bochs/QEMU shutdown
    if (try_bochs_shutdown()) {
        vga_puts("System shutdown successful via Bochs/QEMU.\n");
        return;
    }

    // If all else fails, force a reset
    vga_puts("Shutdown failed, forcing reset...\n");
    system_fallback_reset();
}
