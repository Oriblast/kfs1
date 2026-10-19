; Constantes de l'en-tête Multiboot 1
MBALIGN     equ 1 << 0
MEMINFO     equ 1 << 1
FLAGS       equ MBALIGN | MEMINFO
MAGIC       equ 0x1BADB002
CHECKSUM    equ -(MAGIC + FLAGS)

; En-tête que GRUB doit trouver dans le binaire du noyau.
; Le script de linkage devra placer cette section correctement.
section .multiboot
align 4
    dd MAGIC
    dd FLAGS
    dd CHECKSUM

; Zone mémoire réservée à la pile, non initialisée dans le fichier.
section .bss
align 16
stack_bottom:
    resb 16384               ; Réserve 16 Kio
stack_top:                   ; Adresse juste après la zone réservée

; Instructions exécutables.
section .text
global _start
extern kmain                 ; Fonction définie dans un autre fichier objet

_start:
    mov esp, stack_top       ; Initialise le pointeur de pile
    call kmain               ; Appelle le code C du noyau

    cli                      ; Désactive les interruptions masquables
.hang:
    hlt                      ; Met le processeur en attente
    jmp .hang                ; Recommence si le processeur se réveille