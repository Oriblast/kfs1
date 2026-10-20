#ifndef IO_H
#define IO_H

#include "types.h"

/* Écrit un octet sur un port d'entrée/sortie (instruction x86 "out") */
static inline void outb(uint16_t port, uint8_t value) {
    __asm__ __volatile__("outb %0, %1" : : "a"(value), "Nd"(port));
}

/* Lit un octet depuis un port d'entrée/sortie (instruction x86 "in") */
static inline uint8_t inb(uint16_t port) {
    uint8_t value;
    __asm__ __volatile__("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

#endif
