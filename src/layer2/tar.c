#include <mibiib/tar.h>
/* --- TAR FILESYSTEM PARSER --- */

struct tar_header {
    char filename[100];
    char mode[8];
    char uid[8];
    char gid[8];
    char size[12];
    char mtime[12];
    char chksum[8];
    char typeflag;
};
// Helper: Convert Octal string to Integer
uint32_t oct2bin(const char* str, int size) {
    uint32_t n = 0;
    const unsigned char* c = (const unsigned char*)str;
    while (size-- > 0) {
        n *= 8;
        n += *c - '0';
        c++;
    }
    return n;
}

void load_tar(uint64_t address) {
    struct tar_header* header = (struct tar_header*)address;
    
    while (1) {
        if (header->filename[0] == 0) break;

        // --- NEW FIX: Strip "./" prefix if present ---
        char* safe_name = header->filename;
        if (safe_name[0] == '.' && safe_name[1] == '/') {
            safe_name += 2; // Skip the first two characters
        }
        // ---------------------------------------------

        uint32_t size = oct2bin(header->size, 11);

        // Update logic to use safe_name instead of header->filename
        if (header->typeflag == '5') {
            fs_mkdir(fs_root, safe_name); 
        } else {
            char* file_data = (char*)header + 512;
            fs_mkfile(fs_root, safe_name, file_data);
        }
        term_print(header->filename);
        term_print("\n");

        // Jump to next header
        // Address + 512 (header) + size (aligned to 512)
        uint64_t offset = 512 + size;
        if (size % 512 != 0) {
            offset += 512 - (size % 512);
        }
        header = (struct tar_header*)((char*)header + offset);
    }
}