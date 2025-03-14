#ifndef MM_H
#define MM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Memory frame size (4 KB)
#define FRAME_SIZE 4096

// Memory manager functions
void mm_init(uint32_t* bitmap_start, uint64_t total_memory_bytes);
uint32_t* mm_alloc_frame(void);
void mm_free_frame(uint32_t* frame);
size_t mm_get_total_frames(void);
size_t mm_get_used_frames(void);
size_t mm_get_free_frames(void);

#endif // MM_H
