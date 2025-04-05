#include "../../include/video/vga.h"
#include "../../include/boot/multiboot.h"
#include "../../include/mm/vmm.h"
#include "../../include/kernel/panic/panic.h"
#include "../../include/keyboard/kb.h"
#include "../../include/shell/shell.h"
#include "../../include/kernel/panic/debug.h"
#include "../../include/kernel/arch/x86/cpu.h"

extern uint32_t multiboot_info_ptr;
extern uint32_t __bitmap_start;

static void delay(uint32_t milliseconds) {
    for (uint32_t i = 0; i < milliseconds * 1000; i++) {
        __asm__ volatile ("nop");
    }
}

void boot_screen(void) {
    // Initialize debug system first
    DEBUG_INIT();
    
    // Initialize VGA with black background
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    if (vga_initialize() != 0) {
        DEBUG_ERROR("Failed to initialize VGA");
        panic("Failed to initialize VGA in the booting process\n");
    }
    DEBUG_SUCCESS("VGA initialized");
    delay(100000);
    
    // Initialize CPU early (basic identification)
    cpu_early_init();
    
    // Verify CPU is working by performing some basic tests
    DEBUG_INFO("Verifying CPU functionality...");
    
    // Test 1: Check if CPUID is working
    uint32_t eax, ebx, ecx, edx;
    cpu_cpuid(0, 0, &eax, &ebx, &ecx, &edx);
    if (eax == 0 && ebx == 0 && ecx == 0 && edx == 0) {
        DEBUG_ERROR("CPUID instruction failed");
        panic("CPU verification failed: CPUID instruction not working\n");
    }
    DEBUG_SUCCESS("CPUID instruction verified");
    
    // Test 2: Check if basic registers are working
    uint32_t test_value = 0x12345678;
    uint32_t read_value;
    asm volatile (
        "mov %1, %%eax\n\t"
        "mov %%eax, %0\n\t"
        : "=r" (read_value)
        : "r" (test_value)
        : "%eax"
    );
    if (read_value != test_value) {
        DEBUG_ERROR("Register test failed");
        panic("CPU verification failed: Basic register operations not working\n");
    }
    DEBUG_SUCCESS("Register operations verified");
    
    // Test 3: Check if TSC is working (if available)
    cpu_info_t cpu_info;
    cpu_identify(&cpu_info);
    if (cpu_info.features.tsc) {
        uint64_t tsc1 = cpu_rdtsc();
        delay(1000); // Small delay
        uint64_t tsc2 = cpu_rdtsc();
        if (tsc2 <= tsc1) {
            DEBUG_ERROR("TSC not incrementing");
            panic("CPU verification failed: TSC not working properly\n");
        }
        DEBUG_SUCCESS("TSC verified");
    } else {
        DEBUG_INFO("TSC not available, skipping test");
    }
    
    // Initialize Virtual Memory Manager
    DEBUG_INFO("Initializing Virtual Memory Manager");
    struct multiboot_info* mb_info = (struct multiboot_info*)multiboot_info_ptr;
    uint64_t total_memory = (mb_info->mem_upper + 1024) * 1024; // Convert to bytes
    
    vmm_init((uint32_t*)&__bitmap_start, total_memory);
    DEBUG_SUCCESS("VMM initialized with %d MB RAM", total_memory / (1024 * 1024));
    delay(100000);
    
    // Initialize Keyboard
    if (kb_init() != 0) {
        DEBUG_ERROR("Failed to initialize Keyboard");
        panic("Failed to initialize Keyboard in the booting process\n");
    }
    DEBUG_SUCCESS("Keyboard initialized");
    delay(100000);
    
    // Initialize Shell should never check for return value
    DEBUG_INFO("Initializing shell subsystem");
    DEBUG_SUCCESS("Shell initialized");
    delay(100000);
    
    // Display kernel address
    DEBUG_INFO("Kernel running at hex: 0xC0000000");
    delay(200000);
    
    // Display CPU information
    DEBUG_SUCCESS("CPU verified and running");
    DEBUG_INFO("CPU Vendor: %s", cpu_info.identity.vendor_id);
    DEBUG_INFO("CPU Model: %s", cpu_info.identity.brand_string);
    
    // Complete CPU initialization
    cpu_late_init();
    DEBUG_SUCCESS("Cpu0 fully initialized");
    delay(200000);
    
    // Mark boot complete AFTER all hardware is initialized
    kb_set_boot_complete(true);
    
    // Clear the screen and ensure black background
    vga_set_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
    vga_clear();
    
    // Enable keyboard input LAST
    kb_enable_input(true);
    
    // Don't show prompt here - let main() handle the banner first
}
