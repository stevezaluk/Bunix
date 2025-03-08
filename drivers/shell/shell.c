#include "../../include/shell/shell.h"
#include "../../include/video/vga.h"
#include "../../include/keyboard/kb.h"
#include "../../include/lib/string.h"
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

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
    vga_puts("  time=de - Display time in Germany (CET/CEST)\n");
    vga_puts("  time=us - Display time in the US (EST/EDT)\n");
}

// Command: clear
static void shell_clear(void) {
    vga_clear(); // Clear the screen (resets cursor to 0,0)
}

// Command: echo
static void shell_echo(const char *text) {
    if (text == NULL || *text == '\0') {
        vga_puts("Usage: echo <text>\n");
        return;
    }
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

// Command: time=de or time=us
static void shell_time(const char *timezone) {
    if (timezone == NULL || (*timezone != 'd' && *timezone != 'u')) {
        vga_puts("Usage: time=de or time=us\n");
        return;
    }

    unsigned char hour, minute, second;
    unsigned char day, month, year;
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

    // Read day (register 0x07 for day of month)
    __asm__ volatile (
        "mov $0x07, %%al\n\t"
        "outb %%al, $0x70\n\t"
        "inb $0x71, %%al"
        : "=a"(day)
        :
        : "memory"
    );

    // Read month (register 0x08 for month)
    __asm__ volatile (
        "mov $0x08, %%al\n\t"
        "outb %%al, $0x70\n\t"
        "inb $0x71, %%al"
        : "=a"(month)
        :
        : "memory"
    );

    // Read year (register 0x09 for year)
    __asm__ volatile (
        "mov $0x09, %%al\n\t"
        "outb %%al, $0x70\n\t"
        "inb $0x71, %%al"
        : "=a"(year)
        :
        : "memory"
    );

    // Convert BCD to binary (if needed)
    if (!(statusB & 0x04)) { // If BCD mode is enabled (bit 2 = 0)
        hour = (hour >> 4) * 10 + (hour & 0x0F);
        minute = (minute >> 4) * 10 + (minute & 0x0F);
        second = (second >> 4) * 10 + (second & 0x0F);
        day = (day >> 4) * 10 + (day & 0x0F);
        month = (month >> 4) * 10 + (month & 0x0F);
        year = (year >> 4) * 10 + (year & 0x0F);
    }

    // Adjust for 12-hour mode (if enabled)
    if (!(statusB & 0x02)) { // If 12-hour mode (bit 1 = 0)
        if (hour & 0x80) {   // Check PM bit
            hour = (hour & 0x7F) + 12;
            if (hour == 24) hour = 12;
        }
    }

    // Display UTC time
    vga_puts("UTC time: ");
    vga_putchar((hour / 10) + '0');
    vga_putchar((hour % 10) + '0');
    vga_putchar(':');
    vga_putchar((minute / 10) + '0');
    vga_putchar((minute % 10) + '0');
    vga_putchar(':');
    vga_putchar((second / 10) + '0');
    vga_putchar((second % 10) + '0');
    vga_puts("\n");

    // Adjust time based on the requested time zone
    int local_hour = hour;
    const char *timezone_name = "Local";

    if (strcmp(timezone, "de") == 0) {
        // Germany: CET (UTC+1) or CEST (UTC+2)
        bool is_cest = false;
        if (month > 3 && month < 10) {
            is_cest = true; // Definitely CEST
        } else if (month == 3) {
            // Check if the current day is after the last Sunday in March
            int last_sunday = 31 - (5 + year + year / 4) % 7; // Zeller's congruence for last Sunday
            if (day > last_sunday) is_cest = true;
        } else if (month == 10) {
            // Check if the current day is before the last Sunday in October
            int last_sunday = 31 - (5 + year + year / 4) % 7; // Zeller's congruence for last Sunday
            if (day < last_sunday) is_cest = true;
        }
        local_hour += (is_cest ? 2 : 1); // CEST is UTC+2, CET is UTC+1
        timezone_name = is_cest ? "CEST" : "CET";
    } else if (strcmp(timezone, "us") == 0) {
        // US: EST (UTC-5) or EDT (UTC-4)
        bool is_edt = false;
        if (month > 3 && month < 11) {
            is_edt = true; // Definitely EDT
        } else if (month == 3) {
            // Check if the current day is after the second Sunday in March
            int second_sunday = 14 - (1 + year + year / 4) % 7; // Zeller's congruence for second Sunday
            if (day > second_sunday) is_edt = true;
        } else if (month == 11) {
            // Check if the current day is before the first Sunday in November
            int first_sunday = 7 - (1 + year + year / 4) % 7; // Zeller's congruence for first Sunday
            if (day < first_sunday) is_edt = true;
        }
        local_hour += (is_edt ? -4 : -5); // EDT is UTC-4, EST is UTC-5
        timezone_name = is_edt ? "EDT" : "EST";
    }

    // Handle overflow/underflow in local_hour
    if (local_hour >= 24) local_hour -= 24;
    if (local_hour < 0) local_hour += 24;

    // Display local time
    vga_puts(timezone_name);
    vga_puts(" time: ");
    vga_putchar((local_hour / 10) + '0');
    vga_putchar((local_hour % 10) + '0');
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
            } else if (strncmp(input, "time=", 5) == 0) {
                // Extract the timezone (e.g., "de" or "us")
                const char *timezone = input + 5;
                shell_time(timezone);
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
