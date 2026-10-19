; --- boot.asm ---

; Constantes pour l'en-tête Multiboot
MBALIGN     equ  1 << 0             ; Aligner les modules chargés sur les pages de la mémoire
MEMINFO     equ  1 << 1             ; Fournir une carte mémoire (memory map)
FLAGS       equ  MBALIGN | MEMINFO  ; Indicateurs multiboot combinés
MAGIC       equ  0x1BADB002         ; Nombre magique (identifiant multiboot)
CHECKSUM    equ -(MAGIC + FLAGS)    ; Somme de contrôle pour prouver la validité

; Déclaration de l'en-tête multiboot (doit être au tout début du binaire)
section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

; Réservation d'une pile (stack) pour le noyau
section .bss
align 16
stack_bottom:
    resb 16384                      ; Réserve 16 Ko pour la pile
stack_top:

; Code principal du boot
section .text
global _start
extern kmain                        ; On déclarera notre fonction principale en C/C++ plus tard

_start:
    ; 1. Initialisation du pointeur de pile (ESP)
    mov esp, stack_top

    ; 2. Appel de la fonction principale du noyau
    call kmain

    ; 3. Si le noyau s'arrête par inadvertance, on bloque le processeur dans une boucle infinie
    cli
.hang:
    hlt
    jmp .hang