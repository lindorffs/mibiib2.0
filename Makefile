# ==============================
# My64 OS Makefile
# ==============================

# Tools
CC      = gcc
LD      = ld
NASM    = nasm
GRUBMK  = grub-mkrescue

# Flags
CFLAGS  = -Iinclude/ -m64 -ffreestanding -mno-red-zone -mcmodel=large -fno-pie -Wall -Wextra
LDFLAGS = -n -T linker.ld -z max-page-size=0x1000

# Directories
ISO_DIR = isodir
BOOT_DIR = $(ISO_DIR)/boot
GRUB_DIR = $(BOOT_DIR)/grub

# Files
C_SOURCES = $(wildcard src/kernel/*.c) $(wildcard src/layer2/*.c)
C_OBJECTS = $(C_SOURCES:.c=.o)

ASM_SOURCE = src/boot/boot.asm
ASM_OBJECT = $(ASM_SOURCE:.asm=.o)

KERNEL_BIN = build/bins/kernel.bin
ISO_IMAGE  = build/iso/myos64.iso

# ==============================
# Default Target
# ==============================

all: $(ISO_IMAGE)

# ==============================
# Compile Rules
# ==============================

%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

$(ASM_OBJECT): $(ASM_SOURCE)
	$(NASM) -f elf64 $< -o $@

# ==============================
# Link Kernel
# ==============================

$(KERNEL_BIN): $(ASM_OBJECT) $(C_OBJECTS)
	$(LD) $(LDFLAGS) -o $@ $^

# ==============================
# Build ISO
# ==============================

$(ISO_IMAGE): $(KERNEL_BIN)
	mkdir -p $(GRUB_DIR)
	cp $(KERNEL_BIN) $(BOOT_DIR)/kernel.bin
	echo 'set timeout=0'                         >  $(GRUB_DIR)/grub.cfg
	echo 'set default=0'                         >> $(GRUB_DIR)/grub.cfg
	echo 'menuentry "Mibiib OS [Lynx Kernel]" {'                 >> $(GRUB_DIR)/grub.cfg
	echo '    multiboot2 /boot/kernel.bin'       >> $(GRUB_DIR)/grub.cfg
	echo '    boot'                              >> $(GRUB_DIR)/grub.cfg
	echo '}'                                     >> $(GRUB_DIR)/grub.cfg
	$(GRUBMK) -o $@ $(ISO_DIR)

# ==============================
# Run in QEMU
# ==============================

run: $(ISO_IMAGE)
	qemu-system-x86_64 -cdrom $(ISO_IMAGE)

# ==============================
# Clean
# ==============================

clean:
	rm $(ASM_OBJECT) $(C_OBJECTS) $(KERNEL_BIN) $(ISO_IMAGE)
	rm -r $(ISO_DIR)

# ==============================
# Phony Targets
# ==============================

.PHONY: all run clean
