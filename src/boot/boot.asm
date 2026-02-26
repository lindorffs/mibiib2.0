; boot.asm - Fixed for InitRD
global start
extern kmain

section .multiboot_header
align 8
header_start:
    dd 0xE85250D6                ; Magic number (Multiboot 2)
    dd 0                         ; Architecture 0 (i386)
    dd header_end - header_start ; Header length
    dd 0x100000000 - (0xE85250D6 + 0 + (header_end - header_start)) ; Checksum
    dw 0, 0
    dd 8
header_end:

section .text
bits 32
start:
    ; 1. Set Stack Pointer
    mov esp, stack_top

    ; 2. SAVE MULTIBOOT POINTER
    ; GRUB puts the info pointer in EBX. 
    ; We move it to ESI to keep it safe because 'rep stosd' below will destroy EDI.
    mov esi, ebx 
	mov ebp, eax
	
	; 3. LOAD CR3
    ; We must tell the CPU where the top-level page table (L4) is.
    mov eax, page_table_l4
    mov cr3, eax
	
    ; 4. Map P4 -> P3
    mov eax, page_table_l3
    or eax, 0b11 ; Present | Writable
    mov [page_table_l4], eax

    ; 5. Map P3 -> P2
    mov eax, page_table_l2
    or eax, 0b11 ; Present | Writable
    mov [page_table_l3], eax

    ; 6. Map P2 (Huge Pages) - Map first 1GB
    mov ecx, 0
    mov eax, 0
    or eax, 0b10000011 ; Present | Writable | Huge
.map_loop:
    mov [page_table_l2 + ecx * 8], eax
    add eax, 0x200000
    inc ecx
    cmp ecx, 512
    jne .map_loop

    ; 7. Enable PAE
    mov eax, cr4
    or eax, 1 << 5
    mov cr4, eax

    ; 8. Long Mode Bit
    mov ecx, 0xC0000080
    rdmsr
    or eax, 1 << 8
    wrmsr

    ; 9. Enable Paging
    mov eax, cr0
    or eax, 1 << 31
    mov cr0, eax

    ; 10. Load 64-bit GDT
    lgdt [gdt64.pointer]

    ; 11. Jump to 64-bit Code
    jmp gdt64.code:long_mode_start

bits 64
long_mode_start:
    ; 12. Clear Segments
    mov ax, 0
    mov ss, ax
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax

    ; 13. RESTORE MULTIBOOT POINTER
    ; The System V AMD64 ABI requires the first argument to be in RDI.
    ; We saved the pointer in ESI back in 32-bit mode.
    mov edi, esi
	mov esi, ebp

    ; 14. Call Kernel
    call kmain

    cli
    hlt

section .rodata
gdt64:
    dq 0
.code: equ $ - gdt64
    dq (1<<43) | (1<<44) | (1<<47) | (1<<53)
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