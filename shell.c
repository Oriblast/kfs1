#include "shell.h"
#include "types.h"
#include "string.h"
#include "printk.h"
#include "vga.h"
#include "keyboard.h"
#include "gdt.h"
#include "stack.h"
#include "io.h"

#define PROMPT   "kfs> "
#define LINE_MAX 72

typedef struct {
    char buf[LINE_MAX + 1];
    int len;
} line_t;

static line_t lines[VGA_SCREENS];

typedef struct {
    const char* name;
    const char* help;
    void (*run)(const char* args);
} command_t;

static void cmd_help(const char* args);
static void cmd_clear(const char* args);
static void cmd_echo(const char* args);
static void cmd_stack(const char* args);
static void cmd_gdt(const char* args);
static void cmd_layout(const char* args);
static void cmd_reboot(const char* args);
static void cmd_halt(const char* args);

static const command_t commands[] = {
    { "help",   "liste les commandes",                cmd_help },
    { "clear",  "efface l'ecran",                     cmd_clear },
    { "echo",   "affiche le texte donne",             cmd_echo },
    { "stack",  "affiche la pile du noyau",           cmd_stack },
    { "gdt",    "affiche la GDT chargee dans le CPU", cmd_gdt },
    { "layout", "qwerty | azerty",                    cmd_layout },
    { "reboot", "redemarre la machine",               cmd_reboot },
    { "halt",   "arrete le processeur",               cmd_halt },
};

#define COMMAND_COUNT (sizeof(commands) / sizeof(commands[0]))

static void print_prompt(void) {
    terminal_setcolor(vga_color(VGA_LIGHT_GREEN, VGA_BLACK));
    terminal_write(PROMPT);
    terminal_setcolor(vga_color(VGA_LIGHT_GREY, VGA_BLACK));
}

static void print_error(const char* msg, const char* arg) {
    terminal_setcolor(vga_color(VGA_LIGHT_RED, VGA_BLACK));
    printk("%s%s\n", msg, arg);
    terminal_setcolor(vga_color(VGA_LIGHT_GREY, VGA_BLACK));
}

static int is_space(char c) {
    return c == ' ' || c == '\t';
}

static void execute(char* line) {
    while (is_space(*line))
        line++;
    if (*line == '\0')
        return;

    char* args = line;
    while (*args && !is_space(*args))
        args++;
    if (*args) {
        *args++ = '\0';
        while (is_space(*args))
            args++;
    }

    for (size_t i = 0; i < COMMAND_COUNT; i++) {
        if (strcmp(line, commands[i].name) == 0) {
            commands[i].run(args);
            return;
        }
    }
    print_error("commande inconnue : ", line);
}

/* --- Commandes --- */

static void cmd_help(const char* args) {
    (void)args;
    for (size_t i = 0; i < COMMAND_COUNT; i++) {
        terminal_setcolor(vga_color(VGA_YELLOW, VGA_BLACK));
        printk("  %s", commands[i].name);
        terminal_setcolor(vga_color(VGA_LIGHT_GREY, VGA_BLACK));
        for (size_t pad = strlen(commands[i].name); pad < 8; pad++)
            terminal_putchar(' ');
        printk("%s\n", commands[i].help);
    }
    printk("  F1-F%d : changer d'ecran\n", VGA_SCREENS);
}

static void cmd_clear(const char* args) {
    (void)args;
    terminal_clear();
}

static void cmd_echo(const char* args) {
    printk("%s\n", args);
}

static void cmd_stack(const char* args) {
    (void)args;
    print_kernel_stack();
}

static void cmd_gdt(const char* args) {
    (void)args;
    gdt_print();
}

static void cmd_layout(const char* args) {
    if (strcmp(args, "qwerty") == 0)
        keyboard_set_layout(LAYOUT_QWERTY);
    else if (strcmp(args, "azerty") == 0)
        keyboard_set_layout(LAYOUT_AZERTY);
    else if (*args)
        print_error("disposition inconnue : ", args);
    printk("layout : %s\n", keyboard_get_layout() == LAYOUT_AZERTY ? "azerty" : "qwerty");
}

static void cmd_reboot(const char* args) {
    (void)args;
    printk("Redemarrage...\n");
    while (inb(0x64) & 0x02)
;
    outb(0x64, 0xFE);
    struct { uint16_t limit; uint32_t base; } __attribute__((packed)) null_idt = { 0, 0 };
    __asm__ __volatile__("lidt %0; int $3" : : "m"(null_idt));
}

static void cmd_halt(const char* args) {
    (void)args;
    terminal_setcolor(vga_color(VGA_LIGHT_RED, VGA_BLACK));
    printk("Systeme arrete.\n");
    while (1)
        __asm__ __volatile__("cli; hlt");
}

/* --- Saisie --- */

void shell_init(void) {
    for (int i = VGA_SCREENS - 1; i >= 0; i--) {
        terminal_switch(i);
        lines[i].len = 0;
        print_prompt();
    }
}

void shell_handle_key(int key) {
    if (key >= KEY_F1 && key < KEY_F1 + VGA_SCREENS) {
        terminal_switch(key - KEY_F1);
        return;
    }
    if (key >= 0x100)
        return;

    line_t* line = &lines[terminal_current()];
    char c = (char)key;

    if (c == '\n') {
        terminal_putchar('\n');
        line->buf[line->len] = '\0';
        execute(line->buf);
        line->len = 0;
        print_prompt();
    } else if (c == '\b') {
        /* On n'efface que ce qui a été tapé, jamais le prompt */
        if (line->len > 0) {
            line->len--;
            terminal_putchar('\b');
        }
    } else if (c == '\t') {
        return;
    } else if (line->len < LINE_MAX && (uint8_t)c >= 0x20) {
        line->buf[line->len++] = c;
        terminal_putchar(c);
    }
}
