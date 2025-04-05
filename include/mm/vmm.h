#ifndef VMM_H
#define VMM_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

// Page size (4 KB)
#define PAGE_SIZE 4096

// Virtual memory manager functions
void vmm_init(uint32_t* page_bitmap_start, uint64_t total_memory_bytes);
uint32_t* vmm_alloc_page(void);
void vmm_free_page(uint32_t* page);
size_t vmm_get_total_pages(void);
size_t vmm_get_used_pages(void);
size_t vmm_get_free_pages(void);

#endif // VMM_H
