#include "../include/shell/shell.h"
#include "../include/video/vga.h"
#include <stdint.h>

typedef struct {
    uint32_t eax;
    uint32_t ebx;
    uint32_t ecx;
    uint32_t edx;
} cpuid_result_t;

void cpuid(uint32_t eax, uint32_t ecx, cpuid_result_t *result) {
    __asm__ volatile("cpuid"
                     : "=a"(result->eax), "=b"(result->ebx), "=c"(result->ecx), "=d"(result->edx)
                     : "a"(eax), "c"(ecx));
}

void cpuinfo_command(const char *args) {
    cpuid_result_t result;
    char vendor[13] = {0};

    // Get CPU vendor ID
    cpuid(0, 0, &result);
    *(uint32_t *)&vendor[0] = result.ebx;
    *(uint32_t *)&vendor[4] = result.edx;
    *(uint32_t *)&vendor[8] = result.ecx;
    vendor[12] = '\0';

    vga_puts("Vendor ID: ");
    vga_puts(vendor);
    vga_puts("\n");

    // Get CPU features (CPUID function 1)
    cpuid(1, 0, &result);

    // Check architecture (long mode support)
    if (result.edx & (1 << 29)) {
        vga_puts("Architecture: x86_64\n");
    } else {
        vga_puts("Architecture: i386\n");
    }

    // Additional CPU information
    uint32_t family = (result.eax >> 8) & 0xF;
    uint32_t model = (result.eax >> 4) & 0xF;
    uint32_t stepping = result.eax & 0xF;

    vga_puts("Family: ");
    vga_putdec(family, 1);
    vga_puts("\nModel: ");
    vga_putdec(model, 1);
    vga_puts("\nStepping: ");
    vga_putdec(stepping, 1);
    vga_puts("\n");

    // Check for additional features
    if (result.edx & (1 << 23)) {
        vga_puts("Feature: MMX\n");
    }
    if (result.edx & (1 << 25)) {
        vga_puts("Feature: SSE\n");
    }
    if (result.edx & (1 << 26)) {
        vga_puts("Feature: SSE2\n");
    }
    if (result.ecx & (1 << 0)) {
        vga_puts("Feature: SSE3\n");
    }
    if (result.ecx & (1 << 9)) {
        vga_puts("Feature: SSSE3\n");
    }
    if (result.ecx & (1 << 19)) {
        vga_puts("Feature: SSE4.1\n");
    }
    if (result.ecx & (1 << 20)) {
        vga_puts("Feature: SSE4.2\n");
    }
    if (result.ecx & (1 << 28)) {
        vga_puts("Feature: AVX\n");
    }
    // Add more feature checks as needed
}
