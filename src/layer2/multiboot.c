#include <mibiib/grub/multiboot.h>
#include <mibiib/tar.h>

struct multiboot_tag {
    uint32_t type;
    uint32_t size;
};

struct multiboot_tag_module {
    uint32_t type;
    uint32_t size;
    uint32_t mod_start;
    uint32_t mod_end;
    char string[];
};

void init_multiboot(uint64_t addr) {
    term_print("Parsing Multiboot Info at: "); term_print_hex(addr); term_print("\n");

    // The total size of MB info is the first 4 bytes
    // Reserved is next 4 bytes. Tags start at +8.
    uint32_t total_size = *(uint32_t*)addr;
    term_print("Total MBI Size: "); term_print_dec(total_size); term_print("\n");

    uint8_t* ptr = (uint8_t*)(addr + 8);
    struct multiboot_tag* tag = (struct multiboot_tag*)ptr;

    while (tag->type != 0) {
        term_print(" Tag Type: "); term_print_dec(tag->type); 
        term_print(" Size: "); term_print_dec(tag->size); term_print("\n");

        if (tag->type == 3) { // MODULE
            struct multiboot_tag_module* mod = (struct multiboot_tag_module*)tag;
            term_print("  -> MODULE FOUND! Start: "); term_print_hex(mod->mod_start); 
            term_print(" End: "); term_print_hex(mod->mod_end); term_print("\n");
            
            load_tar((uint64_t)mod->mod_start);
        }

        // Align to 8 bytes
        ptr += tag->size;
        if ((uint64_t)ptr % 8 != 0) {
            ptr += 8 - ((uint64_t)ptr % 8);
        }
        tag = (struct multiboot_tag*)ptr;
    }
    term_print("End of Tags.\n");
}