#include "../include/shell/shell.h"
#include "../include/video/vga.h"
#include "../include/mm/mm.h"

// meminfo command implementation
void meminfo_command(const char *args) {
    (void)args; // Unused parameter

    // Get memory information from the memory manager
    size_t total_frames = mm_get_total_frames();
    size_t used_frames = mm_get_used_frames();
    size_t free_frames = mm_get_free_frames();

    // Convert frames to MB (1 frame = 4 KB)
    uint64_t total_memory_mb = (total_frames * FRAME_SIZE) / (1024 * 1024);
    uint64_t used_memory_mb = (used_frames * FRAME_SIZE) / (1024 * 1024);
    uint64_t free_memory_mb = (free_frames * FRAME_SIZE) / (1024 * 1024);

    // Display memory information without leading zeros
    vga_puts("Total Memory: ");
    vga_putdec(total_memory_mb, 0); // Display total memory in MB without padding
    vga_puts(" MB\n");

    vga_puts("Used Memory:  ");
    vga_putdec(used_memory_mb, 0); // Display used memory in MB without padding
    vga_puts(" MB\n");

    vga_puts("Free Memory:  ");
    vga_putdec(free_memory_mb, 0); // Display free memory in MB without padding
    vga_puts(" MB\n");
}
