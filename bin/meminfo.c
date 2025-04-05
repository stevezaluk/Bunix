#include "../include/shell/shell.h"
#include "../include/video/vga.h"
#include "../include/mm/vmm.h"

void meminfo_command(const char *args) {
    (void)args; // Unused parameter
    
    size_t total_pages = vmm_get_total_pages();
    size_t used_pages = vmm_get_used_pages();
    size_t free_pages = vmm_get_free_pages();
    
    uint64_t total_memory_mb = (total_pages * PAGE_SIZE) / (1024 * 1024);
    uint64_t used_memory_mb = (used_pages * PAGE_SIZE) / (1024 * 1024);
    uint64_t free_memory_mb = (free_pages * PAGE_SIZE) / (1024 * 1024);
    
    vga_puts("Memory Information:\n");
    vga_puts("Total Memory: ");
    vga_putdec(total_memory_mb, 0);
    vga_puts(" MB\n");
    vga_puts("Used Memory:  ");
    vga_putdec(used_memory_mb, 0);
    vga_puts(" MB\n");
    vga_puts("Free Memory:  ");
    vga_putdec(free_memory_mb, 0);
    vga_puts(" MB\n");
    vga_puts("Total Pages:  ");
    vga_putdec(total_pages, 0);
    vga_puts("\n");
    vga_puts("Used Pages:   ");
    vga_putdec(used_pages, 0);
    vga_puts("\n");
    vga_puts("Free Pages:   ");
    vga_putdec(free_pages, 0);
    vga_puts("\n");
}
