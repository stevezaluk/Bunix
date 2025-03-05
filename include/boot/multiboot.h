#ifndef MULTIBOOT_H
#define MULTIBOOT_H

#include <stdint.h>

// Multiboot header magic number
#define MULTIBOOT_HEADER_MAGIC 0x1BADB002

// Multiboot header flags
#define MULTIBOOT_HEADER_FLAGS 0x00000003

// Multiboot header structure
struct multiboot_header {
    uint32_t magic;
    uint32_t flags;
    uint32_t checksum;
};

#endif // MULTIBOOT_H
