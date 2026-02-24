ENTRY(start)

SECTIONS
{
    /* Load kernel at 1MB */
    . = 1M;

    .boot :
    {
        /* Ensure Multiboot header is first */
        KEEP(*(.multiboot_header))
    }

    .text :
    {
        *(.text)
    }

    .rodata :
    {
        *(.rodata)
    }

    .data :
    {
        *(.data)
    }

    .bss :
    {
        *(.bss)
    }
}