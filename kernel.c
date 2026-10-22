#include "vga.h"
#include "printk.h"
#include "gdt.h"
#include "stack.h"
#include "keyboard.h"
#include "shell.h"

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
    printk("\nKFS_2 - Kernel from Scratch");
    terminal_setcolor(vga_color(VGA_DARK_GREY, VGA_BLACK));
    printk("   [screen %d/%d]\n", screen + 1, VGA_SCREENS);

    terminal_setcolor(vga_color(VGA_LIGHT_GREEN, VGA_BLACK));
    printk("F1-F%d : change screen   F2 : kernel stack   tape 'help'\n\n", VGA_SCREENS);
    terminal_setcolor(vga_color(VGA_LIGHT_GREY, VGA_BLACK));
}

static void printk_demo(void) {
    terminal_setcolor(vga_color(VGA_LIGHT_MAGENTA, VGA_BLACK));
    printk("printk: %d %i %u | %x %X %08x | %c %s %p %%\n\n",
           42, -42, 42u, 0xcafe, 0xCAFE, 0x42, '*', "str", (void*)VGA_ADDRESS);
    terminal_setcolor(vga_color(VGA_LIGHT_GREY, VGA_BLACK));
}


void kmain(void) {
    init_gdt();
    terminal_init();

    for (int i = VGA_SCREENS - 1; i >= 0; i--) {
        terminal_switch(i);
        print_banner(i);
    }
    printk_demo();

    terminal_switch(1);
    terminal_clear();
    print_kernel_stack();
    terminal_switch(0);

    shell_init();

    /* Pas d'interruptions (pas encore d'IDT) : on interroge le clavier en boucle */
    while (1) {
        int key = keyboard_read();
        if (key == 0)
            __asm__ __volatile__("pause");
        else
            shell_handle_key(key);
    }
}
