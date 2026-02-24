#!/bin/bash
set -e

echo "1. Assembling..."
nasm -f elf64 boot.asm -o boot.o

echo "2. Compiling C sources..."
# Compile kernel.c
gcc -c kernel.c -o kernel.o -m64 -ffreestanding -mno-red-zone -mcmodel=large -fno-pie
# Compile fs.c
gcc -c fs.c -o fs.o -m64 -ffreestanding -mno-red-zone -mcmodel=large -fno-pie

echo "3. Linking..."
# Link boot, kernel, AND fs together
ld -n -o kernel.bin -T linker.ld boot.o kernel.o fs.o -z max-page-size=0x1000

echo "4. ISO..."
mkdir -p isodir/boot/grub
cp kernel.bin isodir/boot/kernel.bin
cat > isodir/boot/grub/grub.cfg << EOF
set timeout=0
set default=0
menuentry "My64 OS" {
    multiboot2 /boot/kernel.bin
    boot
}
EOF
grub-mkrescue -o myos64.iso isodir 2> /dev/null

echo "Run with:"
echo "qemu-system-x86_64 -cdrom myos64.iso"