#include "../include/shell/shell.h"
#include "../include/video/vga.h"

void cpuinfo_command(const char *args) {
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
