#include "../include/shell/shell.h"
#include "../include/video/vga.h"
#include "../include/kernel/rtc/rtc.h"

void date_command(const char *args) {
    struct rtc_date date;
    
    // Read full date information
    rtc_read_full(&date);
    
    // Display the date
    vga_puts("Date: ");
    vga_putdec(date.day, 2);
    vga_putchar('/');
    vga_putdec(date.month, 2);
    vga_putchar('/');
    vga_putdec(2000 + date.year, 4);
    
    // Display day of week if available
    uint8_t weekday = day_of_week(date.day, date.month, date.year);
    vga_puts(" (");
    switch (weekday) {
        case 0: vga_puts("Sun"); break;
        case 1: vga_puts("Mon"); break;
        case 2: vga_puts("Tue"); break;
        case 3: vga_puts("Wed"); break;
        case 4: vga_puts("Thu"); break;
        case 5: vga_puts("Fri"); break;
        case 6: vga_puts("Sat"); break;
        default: vga_puts("???"); break;
    }
    vga_puts(")\n");
}
