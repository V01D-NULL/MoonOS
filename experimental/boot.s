section .multiboot_header
header_start:
    dd 0xE85250D6                 ; Magic number
    dd 0                          ; Architecture type (0 = i386)
    dd header_end - header_start  ; Header length

    ; Checksum
    dd 0x100000000 - (0xe85250d6 + 0 + (header_end - header_start))
    dd 0 ; type
    dd 0 ; flags
    dd 8 ; size
header_end:

global start
section .text
extern kmain

[bits 64]
start:
    call kmain
    hlt