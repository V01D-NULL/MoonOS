ENTRY(_start)

SECTIONS
{
    . = 4M;

    .text : ALIGN(0x1000) {
        *(.text*)
    }

    .rodata : {
        *(.rodata*)
    }

    .data : ALIGN(0x1000) {
        *(.data*)
    }

    .bss : ALIGN(0x1000) {
        *(COMMON)
        *(.bss*)
    }
}