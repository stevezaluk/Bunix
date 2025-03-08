#ifndef SYSCALL_H
#define SYSCALL_H

#include <stddef.h> // for size_t

// Syscall numbers
#define SYS_WRITE 4

// Function prototype for sys_write
int sys_write(int fd, const void *buf, size_t count);

#endif // SYSCALL_H
