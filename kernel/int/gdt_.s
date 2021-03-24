; Loads the 64 bit gdt
[bits 64]

%include "asm/defs.inc"

global _load_gdt

_load_gdt:
    lgdt [rdi]
    push KRNL_CS
    push .flush
    
    .flush:
        mov ax, KRNL_DS
        mov ds, ax
        mov es, ax
        mov gs, ax
        mov ss, ax
        mov fs, ax 
        mov gs, ax
        ret
