#include <kernel/rtc/rtc.h>
#include <stdint.h>
#include <stdbool.h>

// Basic RTC Operations
uint8_t rtc_read_register(uint8_t reg) {
    __asm__ volatile("outb %0, %1" : : "a"(reg), "Nd"(RTC_INDEX_PORT));
    __asm__ volatile("inb %1, %0" : "=a"(reg) : "Nd"(RTC_DATA_PORT));
    return reg;
}

void rtc_write_register(uint8_t reg, uint8_t value) {
    __asm__ volatile("outb %0, %1" : : "a"(reg), "Nd"(RTC_INDEX_PORT));
    __asm__ volatile("outb %0, %1" : : "a"(value), "Nd"(RTC_DATA_PORT));
}

bool rtc_is_updating() {
    return rtc_read_register(RTC_STATUS_A) & RTC_UPDATE_IN_PROGRESS;
}

// Date/Time Functions
void rtc_read_full(struct rtc_date *date) {
    while (rtc_is_updating());  // Wait for update to complete
    
    date->second = rtc_read_register(RTC_SECONDS);
    date->minute = rtc_read_register(RTC_MINUTES);
    date->hour = rtc_read_register(RTC_HOURS);
    date->day = rtc_read_register(RTC_DAY_OF_MONTH);
    date->month = rtc_read_register(RTC_MONTH);
    date->year = rtc_read_register(RTC_YEAR);
    
    uint8_t statusB = rtc_read_register(RTC_STATUS_B);
    date->is_24hour = statusB & RTC_24HOUR_MODE;
    date->is_pm = (date->hour & 0x80) && !date->is_24hour;
    
    if (!(statusB & RTC_BCD_MODE)) {
        date->second = bcd_to_bin(date->second);
        date->minute = bcd_to_bin(date->minute);
        date->hour = bcd_to_bin(date->hour);
        date->day = bcd_to_bin(date->day);
        date->month = bcd_to_bin(date->month);
        date->year = bcd_to_bin(date->year);
    }
}

// Conversion Functions
uint8_t bcd_to_bin(uint8_t bcd) {
    return (bcd >> 4) * 10 + (bcd & 0x0F);
}

uint8_t bin_to_bcd(uint8_t bin) {
    return ((bin / 10) << 4) | (bin % 10);
}

// Date Calculations
bool is_leap_year(uint8_t year) {
    // Year is in 00-99 format (assume 2000-2099)
    uint16_t full_year = 2000 + year;
    return (full_year % 4 == 0 && full_year % 100 != 0) || (full_year % 400 == 0);
}

uint8_t days_in_month(uint8_t month, uint8_t year) {
    static const uint8_t days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && is_leap_year(year)) return 29;
    return days[month - 1];
}

uint8_t day_of_week(uint8_t day, uint8_t month, uint8_t year) {
    if (month < 3) {
        month += 12;
        year--;  // Adjust year for January/February
    }
    uint8_t century = 20;  // Since year is 2000-based (00-99)
    return (day + (13 * (month + 1) / 5 + year + year / 4 + century / 4 + 5 * century)) % 7;
}

// DST Calculations
bool is_dst_eu(uint8_t month, uint8_t day, uint8_t year) {
    if (month < 3 || month > 10) return false;
    if (month > 3 && month < 10) return true;

    // DST starts last Sunday in March
    if (month == 3) {
        uint8_t last_sunday = 31 - (5 + year + year / 4) % 7;
        return day >= last_sunday;
    }

    // DST ends last Sunday in October
    uint8_t last_sunday = 31 - (5 + year + year / 4) % 7;
    return day < last_sunday;
}

bool is_dst_us(uint8_t month, uint8_t day, uint8_t year) {
    if (month < 3 || month > 11) return false;
    if (month > 3 && month < 11) return true;

    // DST starts second Sunday in March
    if (month == 3) {
        uint8_t second_sunday = 14 - (1 + year + year / 4) % 7;
        return day >= second_sunday;
    }

    // DST ends first Sunday in November
    uint8_t first_sunday = 7 - (1 + year + year / 4) % 7;
    return day < first_sunday;
}

// Utility Functions
uint8_t rtc_calculate_last_sunday(uint8_t month, uint8_t year) {
    return days_in_month(month, year) - (5 + year + year / 4) % 7;
}

uint8_t rtc_calculate_nth_weekday(uint8_t month, uint8_t year, uint8_t n, uint8_t weekday) {
    uint8_t first_day = day_of_week(1, month, year);
    uint8_t offset = (weekday - first_day + 7) % 7;
    return 1 + offset + (n - 1) * 7;
}

// Timezone Functions
void rtc_adjust_timezone(struct rtc_date *date, int8_t offset) {
    date->hour += offset;

    // Handle overflow/underflow
    if (date->hour >= 24) {
        date->hour -= 24;
        date->day++;
    } else if (date->hour < 0) {
        date->hour += 24;
        date->day--;
    }
}
