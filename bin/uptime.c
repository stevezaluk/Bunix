#include "../include/kernel/rtc/rtc.h"
#include "../include/video/vga.h"
#include <stdint.h>
#include <stdbool.h>

// Global variable to store the boot time
static struct rtc_date boot_time;

// Function to initialize the boot time
void uptime_init() {
    rtc_read_full(&boot_time);
}

// Function to calculate uptime in seconds
uint32_t calculate_uptime() {
    struct rtc_date current_time;
    rtc_read_full(&current_time);

    // Convert boot time and current time to seconds since epoch
    uint32_t boot_seconds = boot_time.second + boot_time.minute * 60 + boot_time.hour * 3600 +
                            boot_time.day * 86400 + boot_time.month * 2629746 + boot_time.year * 31556952;

    uint32_t current_seconds = current_time.second + current_time.minute * 60 + current_time.hour * 3600 +
                               current_time.day * 86400 + current_time.month * 2629746 + current_time.year * 31556952;

    // Calculate uptime
    return current_seconds - boot_seconds;
}

// Function to display uptime
void uptime_command(const char *args) {
    uint32_t uptime_seconds = calculate_uptime();

    // Convert uptime to days, hours, minutes, and seconds
    uint32_t days = uptime_seconds / 86400;
    uptime_seconds %= 86400;
    uint32_t hours = uptime_seconds / 3600;
    uptime_seconds %= 3600;
    uint32_t minutes = uptime_seconds / 60;
    uptime_seconds %= 60;

    // Display uptime
    vga_puts("Uptime: ");
    vga_putdec(days, 2);  // Display days with 2 digits
    vga_puts(" days, ");
    vga_putdec(hours, 2);  // Display hours with 2 digits
    vga_puts(" hours, ");
    vga_putdec(minutes, 2);  // Display minutes with 2 digits
    vga_puts(" minutes, ");
    vga_putdec(uptime_seconds, 2);  // Display seconds with 2 digits
    vga_puts(" seconds\n");
}
