; Loads the 64 bit gdt
[bits 64]

%include "asm/defs.inc"

global _load_gdt

_load_gdt:
    lgdt [rdi]
    push 0x08
    push .flush
    retfq

    .flush:
        mov ax, 0x10
        mov ds, ax
        mov fs, ax
        mov gs, ax
        mov ss, ax
        mov es, ax
        ret
