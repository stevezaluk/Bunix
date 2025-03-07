#include "../../include/shell/shell.h"
#include "../../include/video/vga.h"
#include "../../include/keyboard/kb.h"
#include "../../include/lib/string.h"
#include <stdbool.h>
#include <stdint.h>  // Add this for uint8_t

// Shell prompt
static const char *SHELL_PROMPT = "Bunix> ";

// Command: help
static void shell_help(void) {
    vga_puts("Available commands:\n");
    vga_puts("  help - Show this help message\n");
    vga_puts("  clear - Clear the screen\n");
    vga_puts("  echo <text> - Print the provided text\n");
    vga_puts("  cpuinfo - Display CPU architecture information\n");
    vga_puts("  reboot - Reboot the system\n");
    vga_puts("  shutdown - Shutdown the system\n");
    vga_puts("  time - Display the current time\n");
}

// Command: clear
static void shell_clear(void) {
    vga_clear(); // Clear the screen (resets cursor to 0,0)
}

// Command: echo
static void shell_echo(const char *text) {
    vga_puts(text);
    vga_puts("\n");
}

// Function to detect CPU architecture
static void detect_cpu_architecture(void) {
    unsigned int eax, ebx, ecx, edx;
    char vendor[13];

    // Get CPU vendor ID
    __asm__ volatile("cpuid"
                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                     : "a"(0));
    *(unsigned int *)&vendor[0] = ebx;
    *(unsigned int *)&vendor[4] = edx;
    *(unsigned int *)&vendor[8] = ecx;
    vendor[12] = '\0';

    // Get CPU features
    __asm__ volatile("cpuid"
                     : "=a"(eax), "=b"(ebx), "=c"(ecx), "=d"(edx)
                     : "a"(1));

    // Check if the CPU supports long mode (x86_64)
    if (edx & (1 << 29)) {
        vga_puts("Architecture: x86_64\n");
    } else {
        vga_puts("Architecture: i386\n");
    }
}

// Command: cpuinfo
static void shell_cpuinfo(void) {
    detect_cpu_architecture();
}

// Command: reboot (real hardware-compatible)
static void shell_reboot(void) {
    vga_puts("Rebooting the system...\n");

    // Try 8042 keyboard controller reset
    uint8_t status;
    do {
        // Wait for the input buffer to be empty
        __asm__ volatile ("inb $0x64, %0" : "=a"(status));
    } while (status & 0x02);

    // Send the reset command
    __asm__ volatile ("outb %0, $0x64" : : "a"((uint8_t)0xFE));

    // If 8042 reset fails, trigger a triple fault
    volatile uint32_t* const reset = (volatile uint32_t*)0xFFFFFFFF;
    *reset = 0;
}

// Command: shutdown (real hardware-compatible)
static void shell_shutdown(void) {
    vga_puts("Shutting down the system...\n");

    // Try ACPI shutdown (port 0x604)
    __asm__ volatile (
        "outw %w0, %w1"
        : 
        : "a" ((uint16_t)0x2000), "Nd" ((uint16_t)0x604)
        : "memory"
    );

    // If ACPI shutdown fails, try Bochs shutdown (port 0xB004)
    __asm__ volatile (
        "outw %w0, %w1"
        : 
        : "a" ((uint16_t)0x2000), "Nd" ((uint16_t)0xB004)
        : "memory"
    );

    // If all else fails, trigger a CPU reset (triple fault)
    volatile uint32_t* const goodnight = (volatile uint32_t*)0xFFFFFFFF;
    *goodnight = 0;
}

// Command: time
static void shell_time(void) {
    unsigned char hour, minute, second;
    unsigned char statusB;

    // Read RTC Status Register B to check BCD and 12/24-hour mode
    __asm__ volatile (
        "mov $0x0B, %%al\n\t"
        "outb %%al, $0x70\n\t"
        "inb $0x71, %%al"
        : "=a"(statusB)
        :
        : "memory"
    );

    // Read hour (register 0x04 for hours, 0x02 for minutes, 0x00 for seconds)
    __asm__ volatile (
        "mov $0x04, %%al\n\t"
        "outb %%al, $0x70\n\t"
        "inb $0x71, %%al"
        : "=a"(hour)
        :
        : "memory"
    );

    // Read minute
    __asm__ volatile (
        "mov $0x02, %%al\n\t"
        "outb %%al, $0x70\n\t"
        "inb $0x71, %%al"
        : "=a"(minute)
        :
        : "memory"
    );

    // Read second
    __asm__ volatile (
        "mov $0x00, %%al\n\t"
        "outb %%al, $0x70\n\t"
        "inb $0x71, %%al"
        : "=a"(second)
        :
        : "memory"
    );

    // Convert BCD to binary (if needed)
    if (!(statusB & 0x04)) { // If BCD mode is enabled (bit 2 = 0)
        hour = (hour >> 4) * 10 + (hour & 0x0F);
        minute = (minute >> 4) * 10 + (minute & 0x0F);
        second = (second >> 4) * 10 + (second & 0x0F);
    }

    // Adjust for 12-hour mode (if enabled)
    if (!(statusB & 0x02)) { // If 12-hour mode (bit 1 = 0)
        if (hour & 0x80) {   // Check PM bit
            hour = (hour & 0x7F) + 12;
            if (hour == 24) hour = 12;
        }
    }

    // Time zone adjustment (example: UTC-5 for EST)
    hour -= 5; // Subtract 5 hours for EST
    if (hour < 0) hour += 24;

    // Display the time
    vga_puts("Current time (EST): ");
    vga_putchar((hour / 10) + '0');
    vga_putchar((hour % 10) + '0');
    vga_putchar(':');
    vga_putchar((minute / 10) + '0');
    vga_putchar((minute % 10) + '0');
    vga_putchar(':');
    vga_putchar((second / 10) + '0');
    vga_putchar((second % 10) + '0');
    vga_puts("\n");
}

// Initialize the shell
void shell_init(void) {
    vga_puts(SHELL_PROMPT);
}

// Run the shell
void shell_run(void) {
    char input[256];
    int index = 0;

    while (1) {
        // Read a character from the keyboard
        char c = kb_getchar();

        // Handle backspace
        if (c == '\b') {
            if (index > 0) {
                index--;
                vga_putchar('\b');
            }
        }
        // Handle enter
        else if (c == '\n') {
            input[index] = '\0';
            vga_puts("\n");

            // Parse the input command
            if (strcmp(input, "help") == 0) {
                shell_help();
            } else if (strcmp(input, "clear") == 0) {
                shell_clear();
            } else if (strncmp(input, "echo ", 5) == 0) {
                shell_echo(input + 5);
            } else if (strcmp(input, "cpuinfo") == 0) {
                shell_cpuinfo();
            } else if (strcmp(input, "reboot") == 0) {
                shell_reboot();
            } else if (strcmp(input, "shutdown") == 0) {
                shell_shutdown();
            } else if (strcmp(input, "time") == 0) {
                shell_time();
            } else if (input[0] != '\0') {
                vga_puts("Unknown command: ");
                vga_puts(input);
                vga_puts("\n");
            }

            // Reset input buffer and reprint prompt
            index = 0;
            vga_puts(SHELL_PROMPT);
        }
        // Handle regular characters
        else {
            if (index < sizeof(input) - 1) {
                input[index++] = c;
                vga_putchar(c);
            }
        }
    }
}
