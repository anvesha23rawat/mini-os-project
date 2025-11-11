# Cross-compiler settings
CC = gcc
ASM = nasm

# Flags
CFLAGS = -std=gnu99 -ffreestanding -O2 -Wall -Wextra -I. -m32 -nostdlib
LDFLAGS = -T linker.ld -ffreestanding -O2 -nostdlib -m32
ASFLAGS = -f elf32

# Source files - ADD kernel/shell.c
KERNEL_SRC = kernel/kernel.c kernel/process.c kernel/demo_processes.c kernel/ml_scheduler.c kernel/fs.c kernel/shell.c
BOOT_SRC = boot/boot.s

# Object files - ADD shell.o
KERNEL_OBJ = kernel.o process.o demo_processes.o ml_scheduler.o fs.o shell.o string.o
BOOT_OBJ = boot.o

# Output files
KERNEL_ELF = mini-os-ml.elf
KERNEL_ISO = mini-os-ml.iso

.PHONY: all clean run

all: $(KERNEL_ISO)

# Kernel objects
kernel.o: kernel/kernel.c
	$(CC) $(CFLAGS) -c kernel/kernel.c -o kernel.o

process.o: kernel/process.c
	$(CC) $(CFLAGS) -c kernel/process.c -o process.o

demo_processes.o: kernel/demo_processes.c
	$(CC) $(CFLAGS) -c kernel/demo_processes.c -o demo_processes.o

ml_scheduler.o: kernel/ml_scheduler.c
	$(CC) $(CFLAGS) -c kernel/ml_scheduler.c -o ml_scheduler.o

# Add fs compilation rule
fs.o: kernel/fs.c
	$(CC) $(CFLAGS) -c kernel/fs.c -o fs.o

# ADD shell compilation rule
shell.o: kernel/shell.c
	$(CC) $(CFLAGS) -c kernel/shell.c -o shell.o

# Compilation rule add karo
string.o: kernel/string.c
	$(CC) $(CFLAGS) -c kernel/string.c -o string.o
	
# Boot loader
boot.o: boot/boot.s
	$(ASM) $(ASFLAGS) $< -o $@

# Link kernel
$(KERNEL_ELF): $(BOOT_OBJ) $(KERNEL_OBJ)
	$(CC) $(LDFLAGS) -o $@ $^

# Create ISO
$(KERNEL_ISO): $(KERNEL_ELF)
	@echo "Creating bootable ISO..."
	mkdir -p isodir/boot/grub
	cp $(KERNEL_ELF) isodir/boot/mini-os-ml.elf 
	cp grub.cfg isodir/boot/grub/
	grub-mkrescue -o $(KERNEL_ISO) isodir
	@echo "ISO created: $(KERNEL_ISO)"

clean:
	@echo "Cleaning build files..."
	rm -f *.o *.elf *.iso
	rm -rf isodir

run: $(KERNEL_ISO)
	@echo "Starting QEMU..."
	qemu-system-i386 -cdrom $(KERNEL_ISO)