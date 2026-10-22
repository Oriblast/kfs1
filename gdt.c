/* --- gdt.c --- */
#include "gdt.h"
#include "printk.h"

#define GDT_ENTRIES 7
#define GDT_ADDRESS ((struct gdt_entry*) 0x00000800)

static struct gdt_ptr gp;

extern void gdt_flush(uint32_t gdt_ptr_addr);

static void gdt_set_gate(int num, uint32_t base, uint32_t limit, uint8_t access, uint8_t gran) {
    
    struct gdt_entry *entry = GDT_ADDRESS + num;

    entry->base_low    = (base & 0xFFFF);
    entry->base_middle = (base >> 16) & 0xFF;
    entry->base_high   = (base >> 24) & 0xFF;
    entry->limit_low   = (limit & 0xFFFF);
    entry->granularity = ((limit >> 16) & 0x0F);
    entry->granularity |= (gran & 0xF0);
    entry->access      = access;
}

void init_gdt(void) {
    gp.limit = (sizeof(struct gdt_entry) * GDT_ENTRIES) - 1;
    gp.base  = 0x00000800;

    gdt_set_gate(0, 0, 0, 0, 0);                 // Null
    gdt_set_gate(1, 0, 0xFFFFF, 0x9A, 0xCF);     // Kernel Code
    gdt_set_gate(2, 0, 0xFFFFF, 0x92, 0xCF);     // Kernel Data
    gdt_set_gate(3, 0, 0xFFFFF, 0x92, 0xCF);     // Kernel Stack
    gdt_set_gate(4, 0, 0xFFFFF, 0xFA, 0xCF);     // User Code
    gdt_set_gate(5, 0, 0xFFFFF, 0xF2, 0xCF);     // User Data
    gdt_set_gate(6, 0, 0xFFFFF, 0xF2, 0xCF);     // User Stack

    gdt_flush((uint32_t)&gp);
}

void print_kernel_stack(void) {
    uint32_t esp, ebp;
    
    __asm__ __volatile__("mov %%esp, %0" : "=r"(esp));
    __asm__ __volatile__("mov %%ebp, %0" : "=r"(ebp));

    printk("=== KERNEL STACK DUMP ===\n");
    printk("ESP: 0x%x | EBP: 0x%x\n", esp, ebp);

    uint32_t *stack_ptr = (uint32_t *)esp;
    for (int i = 0; i < 16; i++) {
        printk("  [ESP + 0x%x] = 0x%x\n", i * 4, stack_ptr[i]);
    }
}