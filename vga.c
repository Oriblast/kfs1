#include "vga.h"
#include "io.h"

#define VGA_MEMORY ((volatile uint16_t*) VGA_ADDRESS)

/* Ports du contrôleur CRT de la carte VGA (registre d'index + registre de données) */
#define CRT_INDEX 0x3D4
#define CRT_DATA  0x3D5

/* Chaque écran virtuel garde sa propre copie du contenu et sa position */
typedef struct {
    uint16_t buffer[VGA_WIDTH * VGA_HEIGHT];
    int row;
    int col;
    uint8_t color;
} screen_t;

static screen_t screens[VGA_SCREENS];
static int current = 0;

static inline uint16_t vga_entry(char c, uint8_t color) {
    /* Cast en uint8_t : char est signé, un caractère >= 128 écraserait la couleur */
    return ((uint16_t)color << 8) | (uint8_t)c;
}

static void cursor_enable(uint8_t start, uint8_t end) {
    outb(CRT_INDEX, 0x0A);
    outb(CRT_DATA, (inb(CRT_DATA) & 0xC0) | start);
    outb(CRT_INDEX, 0x0B);
    outb(CRT_DATA, (inb(CRT_DATA) & 0xE0) | end);
}

static void cursor_update(void) {
    uint16_t pos = screens[current].row * VGA_WIDTH + screens[current].col;

    outb(CRT_INDEX, 0x0F);
    outb(CRT_DATA, (uint8_t)(pos & 0xFF));
    outb(CRT_INDEX, 0x0E);
    outb(CRT_DATA, (uint8_t)(pos >> 8));
}

/* Recopie l'écran actif dans la mémoire vidéo */
static void refresh(void) {
    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        VGA_MEMORY[i] = screens[current].buffer[i];
    cursor_update();
}

static void set_cell(screen_t* s, int x, int y, uint16_t entry) {
    s->buffer[y * VGA_WIDTH + x] = entry;
    if (s == &screens[current])
        VGA_MEMORY[y * VGA_WIDTH + x] = entry;
}

static void screen_clear(screen_t* s) {
    uint16_t blank = vga_entry(' ', s->color);

    for (int i = 0; i < VGA_WIDTH * VGA_HEIGHT; i++)
        s->buffer[i] = blank;
    s->row = 0;
    s->col = 0;
}

/* Remonte tout d'une ligne et vide la dernière */
static void scroll(screen_t* s) {
    uint16_t blank = vga_entry(' ', s->color);

    for (int i = 0; i < VGA_WIDTH * (VGA_HEIGHT - 1); i++)
        s->buffer[i] = s->buffer[i + VGA_WIDTH];
    for (int i = VGA_WIDTH * (VGA_HEIGHT - 1); i < VGA_WIDTH * VGA_HEIGHT; i++)
        s->buffer[i] = blank;
    s->row = VGA_HEIGHT - 1;
    if (s == &screens[current])
        refresh();
}

static void newline(screen_t* s) {
    s->col = 0;
    if (++s->row >= VGA_HEIGHT)
        scroll(s);
}

void terminal_init(void) {
    for (int i = 0; i < VGA_SCREENS; i++) {
        screens[i].color = vga_color(VGA_LIGHT_GREY, VGA_BLACK);
        screen_clear(&screens[i]);
    }
    current = 0;
    cursor_enable(14, 15);
    refresh();
}

void terminal_clear(void) {
    screen_clear(&screens[current]);
    refresh();
}

void terminal_setcolor(uint8_t color) {
    screens[current].color = color;
}

void terminal_putchar(char c) {
    screen_t* s = &screens[current];

    switch (c) {
    case '\n':
        newline(s);
        break;
    case '\r':
        s->col = 0;
        break;
    case '\t':
        s->col = (s->col + 8) & ~7;
        if (s->col >= VGA_WIDTH)
            newline(s);
        break;
    case '\b':
        if (s->col > 0) {
            s->col--;
        } else if (s->row > 0) {
            s->row--;
            s->col = VGA_WIDTH - 1;
        }
        set_cell(s, s->col, s->row, vga_entry(' ', s->color));
        break;
    default:
        set_cell(s, s->col, s->row, vga_entry(c, s->color));
        if (++s->col >= VGA_WIDTH)
            newline(s);
        break;
    }
    cursor_update();
}

void terminal_write(const char* str) {
    while (*str)
        terminal_putchar(*str++);
}

void terminal_switch(int screen) {
    if (screen < 0 || screen >= VGA_SCREENS || screen == current)
        return;
    current = screen;
    refresh();
}

int terminal_current(void) {
    return current;
}
