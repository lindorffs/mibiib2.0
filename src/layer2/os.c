#include <lynx/kernel.h>

#include <mibiib/fs.h>
#include <mibiib/shell.h>
#include <mibiib/grub/multiboot.h>

void kentry(uint64_t mb_info_addr, uint64_t magic) {
	term_clear();
	fs_init();
	
	term_print("Mibiib2 v1.0\n");
	if (mb_info_addr < 0x1000) {
        term_print("CRITICAL ERROR: Invalid Multiboot Pointer!\n");
        term_print("Did boot.asm fail to pass EBX to EDI?\n");
        while(1) asm("hlt");
    }
	term_print("Magic Number: "); term_print_hex(magic); term_print("\n");
    if (magic != 0x36D76289) {
        term_print("WARNING: Invalid Magic Number! (Expected 0x36D76289)\n");
        // If it's 0x2BADB002, you are in Multiboot 1 (QEMU -kernel bug)
    }
	init_multiboot(mb_info_addr);
	
	shell_run();
}