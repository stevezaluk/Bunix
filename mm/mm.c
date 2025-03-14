#include "../include/mm/mm.h"
#include "../include/video/vga.h"

// Declare panic function
void panic(const char *message);

static uint8_t* bitmap = NULL;
static size_t total_frames = 0;
static size_t bitmap_size = 0; // Size of the bitmap in bytes

void mm_init(uint32_t* bitmap_start, uint64_t total_memory_bytes) {
    // Calculate total frames
    total_frames = total_memory_bytes / FRAME_SIZE;

    // Calculate bitmap size (1 bit per frame, rounded up to the nearest byte)
    bitmap_size = (total_frames + 7) / 8;

    // Ensure the bitmap fits in the allocated space
    bitmap = (uint8_t*)bitmap_start;

    // Debug: Print initialization details
    vga_puts("Memory Manager Initialized:\n");
    vga_puts("Total Memory: ");
    vga_putdec(total_memory_bytes / (1024 * 1024), 0); // Convert bytes to MB
    vga_puts(" MB\n");
    vga_puts("Total Frames: ");
    vga_putdec(total_frames, 0);
    vga_puts("\n");
    vga_puts("Bitmap Size: ");
    vga_putdec(bitmap_size, 0);
    vga_puts(" bytes\n");

    // Initialize bitmap to 0 (free)
    for (size_t i = 0; i < bitmap_size; i++) {
        bitmap[i] = 0;
    }
}

uint32_t* mm_alloc_frame(void) {
    // Start from frame 256 (skip first 1MB: 256 frames * 4096 = 1MB)
    const size_t START_FRAME = 256;

    for (size_t i = START_FRAME; i < total_frames; i++) {
        size_t byte_idx = i / 8;
        size_t bit_idx = i % 8;

        if (!(bitmap[byte_idx] & (1 << bit_idx))) {
            bitmap[byte_idx] |= (1 << bit_idx);
            vga_puts("Allocated frame: ");
            vga_putdec(i, 0);
            vga_puts("\n");
            return (uint32_t*)(i * FRAME_SIZE);
        }
    }
    vga_puts("Allocation failed: No free frames!\n");
    return NULL;
}

void mm_free_frame(uint32_t* frame) {
    size_t frame_idx = (size_t)frame / FRAME_SIZE;
    size_t byte_idx = frame_idx / 8;
    size_t bit_idx = frame_idx % 8;

    bitmap[byte_idx] &= ~(1 << bit_idx);
    vga_puts("Freed frame: ");
    vga_putdec(frame_idx, 0);
    vga_puts("\n");
}

size_t mm_get_used_frames(void) {
    size_t used = 0;
    for (size_t i = 0; i < total_frames; i++) {
        size_t byte_idx = i / 8;
        size_t bit_idx = i % 8;
        if (bitmap[byte_idx] & (1 << bit_idx)) used++;
    }
    return used;
}

size_t mm_get_total_frames(void) {
    return total_frames;
}

size_t mm_get_free_frames(void) {
    return total_frames - mm_get_used_frames();
}
