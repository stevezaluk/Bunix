#include "../include/kernel/arch/x86/cpu.h"
#include "../include/video/vga.h"
#include "../include/shell/shell.h"

void cpuinfo_command(const char *args) {
    cpu_info_t cpu;
    cpu_identify(&cpu);

    // Save original color
    uint8_t original_color = vga_get_color();
    enum vga_color old_fg = (enum vga_color)(original_color & 0x0F);
    enum vga_color old_bg = (enum vga_color)((original_color >> 4) & 0x0F);

    // Print basic CPU information
    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_puts("CPU Vendor: ");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_puts(cpu.identity.vendor_id);
    vga_puts(" (");
    vga_puts(cpu_vendor_name(&cpu));
    vga_puts(")\n");

    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_puts("Brand: ");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_puts(cpu.identity.brand_string);
    vga_puts("\n");

    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_puts("Architecture: ");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_puts(cpu.features.lm ? "x86_64" : "i386");
    vga_puts("\n");

    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_puts("Family: ");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_putdec(cpu.identity.family, 0);
    vga_puts("  Model: ");
    vga_putdec(cpu.identity.model, 0);
    vga_puts("  Stepping: ");
    vga_putdec(cpu.identity.stepping, 0);
    vga_puts("\n");

    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_puts("Topology: ");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_puts("Package ");
    vga_putdec(cpu.topology.package_id, 0);
    vga_puts(", Core ");
    vga_putdec(cpu.topology.core_id, 0);
    vga_puts(", Thread ");
    vga_putdec(cpu.topology.smt_id, 0);
    vga_puts("\n");

    vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
    vga_puts("TSC Frequency: ");
    vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
    vga_putdec(cpu.tsc_frequency / 1000, 0);
    vga_puts(" MHz (");
    vga_putdec(cpu.tsc_frequency, 0);
    vga_puts(" KHz)\n");

    // Print feature flags
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_puts("\nInstruction Sets:\n");
    
    #define PRINT_FEATURE(feature, name) \
        vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK); \
        vga_puts("  "); \
        vga_set_color(cpu.features.feature ? VGA_COLOR_LIGHT_GREEN : VGA_COLOR_DARK_GREY, VGA_COLOR_BLACK); \
        vga_puts(name); \
        vga_puts(cpu.features.feature ? " ✓\n" : " ✗\n")

    PRINT_FEATURE(fpu, "FPU");
    PRINT_FEATURE(mmx, "MMX");
    PRINT_FEATURE(sse, "SSE");
    PRINT_FEATURE(sse2, "SSE2");
    PRINT_FEATURE(sse3, "SSE3");
    PRINT_FEATURE(ssse3, "SSSE3");
    PRINT_FEATURE(sse4_1, "SSE4.1");
    PRINT_FEATURE(sse4_2, "SSE4.2");
    PRINT_FEATURE(avx, "AVX");
    PRINT_FEATURE(avx2, "AVX2");
    PRINT_FEATURE(avx512f, "AVX512F");
    PRINT_FEATURE(aes, "AES-NI");
    
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_puts("\nVirtualization:\n");
    PRINT_FEATURE(vmx, "Intel VT-x");
    PRINT_FEATURE(svm, "AMD SVM");
    
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_puts("\nSecurity Features:\n");
    PRINT_FEATURE(smep, "SMEP");
    PRINT_FEATURE(smap, "SMAP");
    PRINT_FEATURE(umip, "UMIP");
    PRINT_FEATURE(nx, "NX/XD");

    // Print cache information
    vga_set_color(VGA_COLOR_YELLOW, VGA_COLOR_BLACK);
    vga_puts("\nCache Hierarchy:\n");
    
    for (uint8_t i = 0; i < cpu.num_caches; i++) {
        const cache_descriptor_t* c = &cpu.caches[i];
        vga_set_color(VGA_COLOR_LIGHT_CYAN, VGA_COLOR_BLACK);
        vga_puts("  L");
        vga_putdec(c->level, 0);
        
        switch(c->type) {
            case CACHE_TYPE_DATA: 
                vga_puts(" Data       ");
                break;
            case CACHE_TYPE_INSTRUCTION: 
                vga_puts(" Instruction");
                break;
            default: 
                vga_puts(" Unified    ");
                break;
        }
        
        vga_set_color(VGA_COLOR_LIGHT_GREEN, VGA_COLOR_BLACK);
        vga_puts("  Size: ");
        vga_putdec(c->size / 1024, 3);
        vga_puts(" KB");
        
        vga_puts("  Line: ");
        vga_putdec(c->line_size, 3);
        vga_puts(" B");
        
        if(c->fully_associative) vga_puts("  Fully-Assoc");
        if(c->inclusive) vga_puts("  Inclusive");
        vga_puts("\n");
    }

    // Restore original color
    vga_set_color(old_fg, old_bg);
}
