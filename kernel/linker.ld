ENTRY(kinit)

PHDRS
{
    null    PT_NULL    FLAGS(0) ;                   /* Null segment */
    text    PT_LOAD    FLAGS((1 << 0) | (1 << 2)) ; /* Execute + Read */
    rodata  PT_LOAD    FLAGS((1 << 2)) ;            /* Read only */
    data    PT_LOAD    FLAGS((1 << 1) | (1 << 2)) ; /* Write + Read */
    dynamic PT_DYNAMIC FLAGS((1 << 1) | (1 << 2)) ; /* Dynamic segment needed for PIE */
}

SECTIONS
{
    /* We wanna be placed in the higher half, 2MiB above 0 in physical memory. */
    . = 0xffffffff80200000;
    kernel_start = .;
    
    /* Then let's place all the other traditional executable sections afterwards. */
    .text : ALIGN(0x1000) {
        *(.text*)
    }

    /* We place the .stivalehdr section containing the header in its own section, */
    /* and we use the KEEP directive on it to make sure it doesn't get discarded. */
    .stivale2hdr : ALIGN(0x1000) {
        KEEP(*(.stivale2hdr))
    }

    .rodata : {
        *(.rodata*)
    }

    .data : ALIGN(0x1000) {
        *(.data*)
    }

    .dynamic : ALIGN(0x1000) {
        *(.dynamic)
    } :data :dynamic

    .bss : ALIGN(0x1000) {
        *(COMMON)
        *(.bss*)
    }
    kernel_end = .;
}
