#include "string.h"

size_t strlen(const char* str) {
    size_t len = 0;
    while (str[len] != '\0')
        len++;
    return len;
}

int strcmp(const char* s1, const char* s2) {
    while (*s1 && *s1 == *s2) {
        s1++;
        s2++;
    }
    return (unsigned char)*s1 - (unsigned char)*s2;
}

void* memset(void* dst, int c, size_t n) {
    uint8_t* d = dst;
    while (n--)
        *d++ = (uint8_t)c;
    return dst;
}

void* memcpy(void* dst, const void* src, size_t n) {
    uint8_t* d = dst;
    const uint8_t* s = src;
    while (n--)
        *d++ = *s++;
    return dst;
}
