# ==============================
# My64 OS Makefile
# ==============================

# Tools
CC      = gcc
LD      = ld
NASM    = nasm
GRUBMK  = grub-mkrescue
TAR     = tar
# Flags
CFLAGS  = -Iinclude/ -m64 -ffreestanding -mno-red-zone -mcmodel=large -fno-pie -Wall -Wextra
LDFLAGS = -n -T linker.ld -z max-page-size=0x1000

# Directories
BUILD_DIR = build
BIN_DIR   = $(BUILD_DIR)/bins
ISO_DIR   = $(BUILD_DIR)/isodir
BOOT_DIR  = $(ISO_DIR)/boot
GRUB_DIR  = $(BOOT_DIR)/grub
ROOTFS    = rootfs

# Files
C_SOURCES = $(wildcard src/kernel/*.c) $(wildcard src/layer2/*.c)
C_OBJECTS = $(C_SOURCES:.c=.o)

ASM_SOURCE = src/boot/boot.asm
ASM_OBJECT = $(ASM_SOURCE:.asm=.o)

KERNEL_BIN = build/bins/kernel.bin
INITRD_TAR = $(BIN_DIR)/initrd.tar
ISO_IMAGE  = build/iso/myos64.iso
all: $(ISO_IMAGE)

# ==============================
# Compile Rules
# ==============================

# Compile C files
%.o: %.c
	$(CC) -c $< -o $@ $(CFLAGS)

# Assemble Bootloader
$(ASM_OBJECT): $(ASM_SOURCE)
	$(NASM) -f elf64 $< -o $@

# ==============================
# Link Kernel
# ==============================

$(KERNEL_BIN): $(ASM_OBJECT) $(C_OBJECTS)
	@mkdir -p $(BIN_DIR)
	$(LD) $(LDFLAGS) -o $@ $^

# ==============================
# Create InitRD (Filesystem)
# ==============================

$(INITRD_TAR):
	@echo ">> Creating RootFS..."
	@mkdir -p $(ROOTFS)
	@echo "Welcome to Mibiib OS" > $(ROOTFS)/welcome.txt
	@echo "This file is loaded from RAM Disk" > $(ROOTFS)/info.txt
	# Create tarball (ustar format is easiest to parse)
	$(TAR) --format=ustar -cf $@ -C $(ROOTFS) --transform='s,^\./,,' .

# ==============================
# Build ISO
# ==============================

$(ISO_IMAGE): $(KERNEL_BIN) $(INITRD_TAR)
	@echo ">> Generating ISO..."
	@mkdir -p $(GRUB_DIR)
	
	# Copy Kernel and InitRD to ISO structure
	cp $(KERNEL_BIN) $(BOOT_DIR)/kernel.bin
	cp $(INITRD_TAR) $(BOOT_DIR)/initrd.tar
	
	# Generate GRUB Config
	echo 'set timeout=0'                          >  $(GRUB_DIR)/grub.cfg
	echo 'set default=0'                          >> $(GRUB_DIR)/grub.cfg
	echo 'menuentry "Mibiib OS [Lynx Kernel]" {'  >> $(GRUB_DIR)/grub.cfg
	echo '    multiboot2 /boot/kernel.bin'        >> $(GRUB_DIR)/grub.cfg
	echo '    module2 /boot/initrd.tar'           >> $(GRUB_DIR)/grub.cfg
	echo '    boot'                               >> $(GRUB_DIR)/grub.cfg
	echo '}'                                      >> $(GRUB_DIR)/grub.cfg
	
	# Make ISO
	$(GRUBMK) -o $@ $(ISO_DIR) 2> /dev/null
	@echo ">> Build Complete: $@"

# ==============================
# Run in QEMU
# ==============================

run: $(ISO_IMAGE)
	qemu-system-x86_64 -cdrom $(ISO_IMAGE)

# ==============================
# Clean
# ==============================

clean:
	rm -f $(ASM_OBJECT) $(C_OBJECTS)
	rm -rf $(BUILD_DIR) $(ROOTFS)

# ==============================
# Phony Targets
# ==============================

.PHONY: all run clean