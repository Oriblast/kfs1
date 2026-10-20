/* Le point d'entrée de notre noyau, défini dans boot.asm */
ENTRY(_start)

SECTIONS
{
    /* On commence à charger le noyau à 1 Mo (1 Megabyte), standard x86 */
    . = 1M;

    /* Section du code et de l'en-tête multiboot */
    .text BLOCK(4K) : ALIGN(4K)
    {
        *(.multiboot)
        *(.text*)
    }

    /* Données en lecture seule (chaînes de caractères constantes, etc.) */
    .rodata BLOCK(4K) : ALIGN(4K)
    {
        *(.rodata*)
    }

    /* Données initialisées (variables globales avec des valeurs de départ) */
    .data BLOCK(4K) : ALIGN(4K)
    {
        *(.data*)
    }

    /* Données non initialisées et pile (stack) */
    .bss BLOCK(4K) : ALIGN(4K)
    {
        *(COMMON)
        *(.bss*)
    }
}