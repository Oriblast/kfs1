#include "types.h"
#include "vga.h"
#include "printk.h"
#include "keyboard.h"

static const char* logo[] = {
    "        :::      ::::::::",
    "      :+:      :+:    :+:",
    "    +:+ +:+         +:+",
    "  +#+  +:+       +#+",
    "+#+#+#+#+#+   +#+",
    "     #+#    #+#",
    "    ###   ########.fr",
};

static void print_banner(int screen) {
    terminal_setcolor(vga_color(VGA_LIGHT_CYAN, VGA_BLACK));
    for (size_t i = 0; i < sizeof(logo) / sizeof(logo[0]); i++)
        printk("%s\n", logo[i]);

    terminal_setcolor(vga_color(VGA_YELLOW, VGA_BLACK));
    printk("\nKFS_1 - Kernel from Scratch");
    terminal_setcolor(vga_color(VGA_DARK_GREY, VGA_BLACK));
    printk("   [screen %d/%d]\n", screen + 1, VGA_SCREENS);

    terminal_setcolor(vga_color(VGA_LIGHT_GREEN, VGA_BLACK));
    printk("F1-F%d : change screen   F12 : QWERTY/AZERTY\n\n", VGA_SCREENS);
    terminal_setcolor(vga_color(VGA_LIGHT_GREY, VGA_BLACK));
}

static void printk_demo(void) {
    terminal_setcolor(vga_color(VGA_LIGHT_MAGENTA, VGA_BLACK));
    printk("printk: %d %i %u | %x %X %08x | %c %s %p %%\n\n",
           42, -42, 42u, 0xcafe, 0xCAFE, 0x42, '*', "str", (void*)VGA_ADDRESS);
    terminal_setcolor(vga_color(VGA_LIGHT_GREY, VGA_BLACK));
}

void kmain(void) {
    terminal_init();

    /* Chaque écran virtuel reçoit sa bannière, puis on revient sur le premier */
    for (int i = VGA_SCREENS - 1; i >= 0; i--) {
        terminal_switch(i);
        print_banner(i);
    }
    printk_demo();

    /* Pas d'interruptions (pas encore d'IDT) : on interroge le clavier en boucle */
    while (1) {
        int key = keyboard_read();

        if (key == 0) {
            __asm__ __volatile__("pause");
        } else if (key >= KEY_F1 && key < KEY_F1 + VGA_SCREENS) {
            terminal_switch(key - KEY_F1);
        } else if (key == KEY_F12) {
            enum keyboard_layout next = keyboard_get_layout() == LAYOUT_QWERTY ? LAYOUT_AZERTY : LAYOUT_QWERTY;
            keyboard_set_layout(next);
            terminal_setcolor(vga_color(VGA_DARK_GREY, VGA_BLACK));
            printk("[layout: %s]\n", next == LAYOUT_AZERTY ? "AZERTY" : "QWERTY");
            terminal_setcolor(vga_color(VGA_LIGHT_GREY, VGA_BLACK));
        } else if (key < 0x100) {
            terminal_putchar((char)key);
        }
    }
}
