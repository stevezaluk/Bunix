// lib/string.c
#include "../include/lib/string.h"

// Calculate the length of a string
size_t strlen(const char *str) {
    const char *s;
    for (s = str; *s; ++s);
    return (s - str);
}

// Compare two strings
int strcmp(const char *s1, const char *s2) {
    while (*s1 && (*s1 == *s2)) {
        s1++;
        s2++;
    }
    return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

// Compare two strings up to n characters
int strncmp(const char *s1, const char *s2, size_t n) {
    while (n && *s1 && (*s1 == *s2)) {
        s1++;
        s2++;
        n--;
    }
    if (n == 0) return 0;
    return (*(unsigned char *)s1 - *(unsigned char *)s2);
}

// Copy a string
char *strcpy(char *dest, const char *src) {
    char *ret = dest;
    while ((*dest++ = *src++));
    return ret;
}

// Copy up to n characters from a string
char *strncpy(char *dest, const char *src, size_t n) {
    char *ret = dest;
    while (n && (*dest++ = *src++)) n--;
    while (n--) *dest++ = '\0';
    return ret;
}

// Concatenate two strings
char *strcat(char *dest, const char *src) {
    char *ret = dest;
    while (*dest) dest++;
    while ((*dest++ = *src++));
    return ret;
}

// Concatenate up to n characters from a string
char *strncat(char *dest, const char *src, size_t n) {
    char *ret = dest;
    while (*dest) dest++;
    while (n-- && (*dest++ = *src++));
    *dest = '\0';
    return ret;
}

// Set n bytes of memory to a specific value
void *memset(void *s, int c, size_t n) {
    unsigned char *p = s;
    while (n--) *p++ = (unsigned char)c;
    return s;
}

// Copy n bytes from one memory location to another
void *memcpy(void *dest, const void *src, size_t n) {
    unsigned char *d = dest;
    const unsigned char *s = src;
    while (n--) *d++ = *s++;
    return dest;
}
