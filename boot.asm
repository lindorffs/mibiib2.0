; boot.asm
global start
extern kmain

section .multiboot_header
align 8
header_start:
    dd 0xE85250D6                ; Magic number (Multiboot 2)
    dd 0                         ; Architecture 0 (i386)
    dd header_end - header_start ; Header length
    dd 0x100000000 - (0xE85250D6 + 0 + (header_end - header_start)) ; Checksum

    ; Required End Tag
    dw 0, 0
    dd 8
header_end:

section .text
bits 32
start:
    ; 1. Set Stack Pointer
    mov esp, stack_top

    ; 2. Clear Page Tables (Crucial to prevent garbage data)
    mov edi, page_table_l4
    mov cr3, edi
    xor eax, eax
    mov ecx, 4096 * 3 ; Clear L4, L3, L2
    rep stosd
    mov edi, page_table_l4 ; Reset pointer

    ; 3. Map P4 -> P3
    mov eax, page_table_l3
    or eax, 0b11 ; Present | Writable
    mov [page_table_l4], eax

    ; 4. Map P3 -> P2
    mov eax, page_table_l2
    or eax, 0b11 ; Present | Writable
    mov [page_table_l3], eax

    ; 5. Map P2 (Huge Pages) - Map first 1GB (512 entries * 2MB)
    mov ecx, 0         ; Counter (0 to 511)
    mov eax, 0         ; Physical Address starting at 0
    or eax, 0b10000011 ; Present | Writable | Huge Page

.map_loop:
    mov [page_table_l2 + ecx * 8], eax ; Write entry
    add eax, 0x200000                  ; Add 2MB to physical address
    inc ecx
    cmp ecx, 512
    jne .map_loop

    ; 6. Enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; 7. Set Long Mode Bit (EFER)
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; 8. Enable Paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; 9. Load 64-bit GDT
    lgdt [gdt64.pointer]

    ; 10. Far Jump to 64-bit Code
    jmp gdt64.code:long_mode_start

bits 64
long_mode_start:
    ; 11. Clear Segment Registers
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; 12. Jump to C Kernel
    call kmain

    ; 13. Safety Halt
    cli
    hlt

section .rodata
gdt64:
    dq 0 ; Zero entry
.code: equ $ - gdt64
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53) ; Code Segment
.pointer:
    dw $ - gdt64 - 1
    dq gdt64

section .bss
align 4096
page_table_l4: resb 4096
page_table_l3: resb 4096
page_table_l2: resb 4096
stack_bottom:  resb 16384
stack_top: