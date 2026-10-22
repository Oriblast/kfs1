#include "stack.h"
#include "types.h"
#include "printk.h"
#include "vga.h"

/* Limites de la pile, définies dans boot.asm */
extern uint8_t stack_bottom[];
extern uint8_t stack_top[];

#define DUMP_MAX_ROWS   8
#define BACKTRACE_DEPTH 16

static int in_stack(uint32_t addr) {
    return addr >= (uint32_t)stack_bottom && addr < (uint32_t)stack_top;
}

/* Une ligne de 16 octets : adresse, 4 mots en hexa, puis les octets en ASCII */
static void dump_row(uint32_t addr, uint32_t end) {
    const uint32_t* words = (const uint32_t*)addr;
    const uint8_t* bytes = (const uint8_t*)addr;

    terminal_setcolor(vga_color(VGA_DARK_GREY, VGA_BLACK));
    printk("%08x  ", addr);
    terminal_setcolor(vga_color(VGA_LIGHT_GREY, VGA_BLACK));
    for (int i = 0; i < 4; i++) {
        if (addr + i * 4 < end)
            printk("%08x ", words[i]);
        else
            printk("         ");
    }
    terminal_setcolor(vga_color(VGA_DARK_GREY, VGA_BLACK));
    printk(" |");
    for (int i = 0; i < 16 && addr + i < end; i++) {
        uint8_t c = bytes[i];
        terminal_putchar(c >= 0x20 && c < 0x7F ? (char)c : '.');
    }
    printk("|\n");
    terminal_setcolor(vga_color(VGA_LIGHT_GREY, VGA_BLACK));
}

/* Contenu brut de la pile, de ESP (sommet actuel) jusqu'à stack_top (base) */
static void stack_dump(uint32_t esp) {
    uint32_t end = (uint32_t)stack_top;
    uint32_t rows = (end - esp + 15) / 16;

    for (uint32_t r = 0; r < rows && r < DUMP_MAX_ROWS; r++)
        dump_row(esp + r * 16, end);
    if (rows > DUMP_MAX_ROWS)
        printk("... %u octets de plus jusqu'a stack_top\n", end - (esp + DUMP_MAX_ROWS * 16));
}

/*
 * Chaque frame commence par l'EBP de l'appelant, suivi de l'adresse de retour :
 *   [ebp]     -> EBP précédent (0 dans _start : fin de la chaîne)
 *   [ebp + 4] -> adresse de retour dans la fonction appelante
 */
static void stack_backtrace(uint32_t ebp) {
    for (int depth = 0; ebp != 0 && depth < BACKTRACE_DEPTH; depth++) {
        if (!in_stack(ebp)) {
            printk("  #%d  EBP 0x%08x hors de la pile, arret\n", depth, ebp);
            return;
        }
        const uint32_t* frame = (const uint32_t*)ebp;
        printk("  #%d  retour 0x%08x   frame 0x%08x\n", depth, frame[1], ebp);
        ebp = frame[0];
    }
}

void print_kernel_stack(void) {
    uint32_t esp, ebp;

    __asm__ __volatile__("mov %%esp, %0" : "=r"(esp));
    __asm__ __volatile__("mov %%ebp, %0" : "=r"(ebp));

    uint32_t size = (uint32_t)(stack_top - stack_bottom);
    uint32_t used = (uint32_t)stack_top - esp;

    terminal_setcolor(vga_color(VGA_YELLOW, VGA_BLACK));
    printk("Kernel stack\n");
    terminal_setcolor(vga_color(VGA_LIGHT_GREY, VGA_BLACK));
    printk("  zone  0x%08x - 0x%08x  (%u Kio)\n", (uint32_t)stack_bottom, (uint32_t)stack_top, size / 1024);
    printk("  ESP   0x%08x   EBP 0x%08x\n", esp, ebp);
    printk("  utilise %u octets, libre %u octets\n\n", used, size - used);

    terminal_setcolor(vga_color(VGA_YELLOW, VGA_BLACK));
    printk("Contenu (ESP -> base)\n");
    stack_dump(esp);

    terminal_setcolor(vga_color(VGA_YELLOW, VGA_BLACK));
    printk("\nBacktrace\n");
    terminal_setcolor(vga_color(VGA_LIGHT_GREY, VGA_BLACK));
    stack_backtrace(ebp);
}
