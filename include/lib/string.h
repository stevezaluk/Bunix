// include/lib/string.h
#ifndef STRING_H
#define STRING_H

#include <stddef.h> // For size_t

int strcmp(const char *s1, const char *s2); // Add this line
int strncmp(const char *s1, const char *s2, size_t n);

#endif // STRING_H
