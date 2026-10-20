# --- Makefile pour KFS_1 ---

CC = gcc
AS = nasm
LD = ld

# Options de compilation strictes pour le noyau (Architecture i386 obligatoire)
CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra \
         -fno-builtin -fno-exceptions -fno-stack-protector \
         -nostdlib -nodefaultlibs -fno-pie -fno-asynchronous-unwind-tables

ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld

OBJ = boot.o kernel.o vga.o string.o printk.o keyboard.o
KERNEL = kernel.bin
ISO = kfs_1.iso

all: $(ISO)

# Compilation de l'assembleur
%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

# Compilation du code C
%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Édition de liens pour créer le binaire du noyau
$(KERNEL): $(OBJ) linker.ld
	$(LD) $(LDFLAGS) $(OBJ) -o $(KERNEL)

# Création de l'image ISO bootable avec GRUB
$(ISO): $(KERNEL) grub.cfg
	mkdir -p iso/boot/grub
	cp $(KERNEL) iso/boot/kernel.bin
	cp grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -d /usr/lib/grub/i386-pc --compress=xz -o $(ISO) iso

# Lancer le noyau avec QEMU (émulateur x86)
run: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -display gtk

clean:
	rm -rf $(OBJ) $(OBJ:.o=.d) $(KERNEL) iso

fclean: clean
	rm -f $(ISO)

re: fclean all

-include $(OBJ:.o=.d)

.PHONY: all clean fclean re run