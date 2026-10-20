#ifndef VGA_H
#define VGA_H

#include "types.h"

#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_SCREENS 4

enum vga_color {
    VGA_BLACK = 0,
    VGA_BLUE = 1,
    VGA_GREEN = 2,
    VGA_CYAN = 3,
    VGA_RED = 4,
    VGA_MAGENTA = 5,
    VGA_BROWN = 6,
    VGA_LIGHT_GREY = 7,
    VGA_DARK_GREY = 8,
    VGA_LIGHT_BLUE = 9,
    VGA_LIGHT_GREEN = 10,
    VGA_LIGHT_CYAN = 11,
    VGA_LIGHT_RED = 12,
    VGA_LIGHT_MAGENTA = 13,
    VGA_YELLOW = 14,
    VGA_WHITE = 15,
};

/* Octet d'attribut : fond sur les 4 bits de poids fort, texte sur les 4 bits de poids faible */
static inline uint8_t vga_color(enum vga_color fg, enum vga_color bg) {
    return (uint8_t)(fg | bg << 4);
}

void terminal_init(void);
void terminal_clear(void);
void terminal_setcolor(uint8_t color);
void terminal_putchar(char c);
void terminal_write(const char* str);
void terminal_switch(int screen);
int  terminal_current(void);

#endif
