ENTRY(_start) /* Define the entry point */

SECTIONS {
    . = 1M; /* Load the kernel at 1MB (conventional for kernels) */

    /* Ensure the multiboot header is at the beginning of the kernel */
    .multiboot : {
        *(.multiboot)
    } :text

    .text : {
        *(.text) /* All text sections (code) */
    } :text /* Assign to the 'text' program header */

    .rodata : {
        *(.rodata) /* Read-only data */
    } :rodata /* Assign to the 'rodata' program header */

    .data : {
        *(.data) /* Initialized data */
    } :data /* Assign to the 'data' program header */

    .bss : {
        *(COMMON) /* Common symbols */
        *(.bss) /* Uninitialized data */
    } :bss /* Assign to the 'bss' program header */
}

/* Define permissions for sections */
PHDRS {
    text PT_LOAD FLAGS(5); /* RX (Read + Execute) */
    rodata PT_LOAD FLAGS(4); /* R (Read-only) */
    data PT_LOAD FLAGS(6); /* RW (Read + Write) */
    bss PT_LOAD FLAGS(6); /* RW (Read + Write) */
}
