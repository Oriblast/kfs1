/* --- kernel.c --- */

typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;

#define VGA_ADDRESS 0xB8000
#define VGA_WIDTH 80
#define VGA_HEIGHT 25

/* Fonction pour effacer l'écran */
void vga_clear(uint8_t color) {
    volatile uint16_t* vga_buffer = (volatile uint16_t*) VGA_ADDRESS;
    uint16_t blank = ((uint16_t)color << 8) | ' ';
    
    for (int y = 0; y < VGA_HEIGHT; y++) {
        for (int x = 0; x < VGA_WIDTH; x++) {
            vga_buffer[y * VGA_WIDTH + x] = blank;
        }
    }
}

void vga_putchar(char c, uint8_t color, int x, int y) {
    volatile uint16_t* vga_buffer = (volatile uint16_t*) VGA_ADDRESS;
    const int index = y * VGA_WIDTH + x;
    vga_buffer[index] = ((uint16_t)color << 8) | (uint16_t)c;
}

void vga_write(const char* str, uint8_t color, int x, int y) {
    int i = 0;
    while (str[i] != '\0') {
        vga_putchar(str[i], color, x + i, y);
        i++;
    }
}

void kmain(void) {
    uint8_t color = 0x0F; // Texte blanc sur fond noir

    /* On efface l'écran proprement au démarrage */
    vga_clear(color);

    /* Affichage de nos messages sur un écran propre */
    vga_write("42 - Kernel from Scratch : KFS_1", color, 0, 0);
    vga_write("Kernel booted successfully !", color, 0, 2);

    while (1) {
        __asm__ __volatile__("hlt");
    }
}