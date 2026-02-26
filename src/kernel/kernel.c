#include <lynx/kernel.h>


void kmain(uint64_t mb_info_addr, uint64_t magic){
    kentry(mb_info_addr, magic);
}

