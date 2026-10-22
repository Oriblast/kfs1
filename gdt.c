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

static const char* gdt_names[GDT_ENTRIES] = {
    "null", "kernel code", "kernel data", "kernel stack",
    "user code", "user data", "user stack",
};

/* Relit la GDT telle que le CPU la voit : adresse donnée par sgdt, registres de segment */
void gdt_print(void) {
    struct gdt_ptr gdtr;
    uint16_t cs, ds, ss;

    __asm__ __volatile__("sgdt %0" : "=m"(gdtr));
    __asm__ __volatile__("mov %%cs, %0" : "=r"(cs));
    __asm__ __volatile__("mov %%ds, %0" : "=r"(ds));
    __asm__ __volatile__("mov %%ss, %0" : "=r"(ss));

    printk("GDTR base 0x%08x  limite %u  (%u entrees)\n", gdtr.base, gdtr.limit, (gdtr.limit + 1) / 8);
    printk("CS 0x%02x  DS 0x%02x  SS 0x%02x\n", cs, ds, ss);
    printk("sel  base       limite     access flags ring type\n");

    const struct gdt_entry* table = (const struct gdt_entry*)gdtr.base;
    for (uint32_t i = 0; i < (gdtr.limit + 1u) / 8 && i < GDT_ENTRIES; i++) {
        const struct gdt_entry* e = &table[i];
        uint32_t base = e->base_low | (uint32_t)e->base_middle << 16 | (uint32_t)e->base_high << 24;
        uint32_t limit = e->limit_low | (uint32_t)(e->granularity & 0x0F) << 16;
        if (e->granularity & 0x80)
            limit = limit << 12 | 0xFFF;

        printk("0x%02x 0x%08x 0x%08x 0x%02x   0x%x   %d    %s\n",
               i * 8, base, limit, e->access, e->granularity >> 4,
               (e->access >> 5) & 3, gdt_names[i]);
    }
}
