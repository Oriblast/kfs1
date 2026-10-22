CC = gcc
AS = nasm
LD = ld

CFLAGS = -m32 -ffreestanding -O2 -Wall -Wextra \
         -fno-builtin -fno-exceptions -fno-stack-protector \
         -nostdlib -nodefaultlibs -fno-pie -fno-asynchronous-unwind-tables

ASFLAGS = -f elf32
LDFLAGS = -m elf_i386 -T linker.ld

OBJ = boot.o kernel.o vga.o string.o printk.o keyboard.o gdt.o gdt_flush.o
KERNEL = kernel.bin
ISO = kfs_1.iso

all: $(ISO)

%.o: %.asm
	$(AS) $(ASFLAGS) $< -o $@

%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

$(KERNEL): $(OBJ) linker.ld
	$(LD) $(LDFLAGS) $(OBJ) -o $(KERNEL)

$(ISO): $(KERNEL) grub.cfg
	mkdir -p iso/boot/grub
	cp $(KERNEL) iso/boot/kernel.bin
	cp grub.cfg iso/boot/grub/grub.cfg
	grub-mkrescue -d /usr/lib/grub/i386-pc --compress=xz -o $(ISO) iso

run: $(ISO)
	qemu-system-i386 -cdrom $(ISO) -display gtk

clean:
	rm -rf $(OBJ) $(OBJ:.o=.d) $(KERNEL) iso

fclean: clean
	rm -f $(ISO)

re: fclean all

-include $(OBJ:.o=.d)

.PHONY: all clean fclean re run