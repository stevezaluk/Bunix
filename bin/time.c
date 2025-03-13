#include "../include/shell/shell.h"
#include "../include/video/vga.h"
#include "../include/kernel/rtc/rtc.h"
#include "../include/lib/string.h"

void display_time(const struct rtc_date *date, const char *label) {
    vga_puts(label);
    vga_putchar(' ');
    vga_putdec(date->hour, 2);
    vga_putchar(':');
    vga_putdec(date->minute, 2);
    vga_putchar(':');
    vga_putdec(date->second, 2);

    if (!date->is_24hour) {
        vga_puts(date->is_pm ? " PM" : " AM");
    }
    vga_puts("\n");
}

void time_command(const char *args) {
    struct rtc_date date;
    rtc_read_full(&date);

    // Display UTC time
    display_time(&date, "UTC Time:");

    // Handle timezone arguments
    if (args != NULL) {
        struct rtc_date local = date;

        if (strcmp(args, "de") == 0) {
            // Germany timezone (CET/CEST)
            int offset = is_dst_eu(local.month, local.day, local.year) ? 2 : 1;
            rtc_adjust_timezone(&local, offset);
            display_time(&local, is_dst_eu(local.month, local.day, local.year) ? 
                        "CEST Time:" : "CET Time:");
        }
        else if (strcmp(args, "us") == 0) {
            // US Eastern timezone (EST/EDT)
            int offset = is_dst_us(local.month, local.day, local.year) ? -4 : -5;
            rtc_adjust_timezone(&local, offset);
            display_time(&local, is_dst_us(local.month, local.day, local.year) ?
                        "EDT Time:" : "EST Time:");
        }
        else {
            vga_puts("Invalid timezone. Use 'de' for Germany or 'us' for US\n");
        }
    }
}
