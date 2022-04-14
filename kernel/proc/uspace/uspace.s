bits 64
section .text

%include "asm/defs.inc"

global enter_ring3_sysret
enter_ring3_sysret:
    ; Setup user stack..
    add rsi, 8192
    mov rbp, rsi
    mov rsp, rbp

    ; NOTE: I'm not setting SS or CS as these are automatically set by sysret
    ; Set user segment selectors
    mov ax, 0x3B
    mov ds, ax
    mov fs, ax
    mov gs, ax
    mov es, ax

    ; Enter ring3
    mov rcx, rdi   ; RIP
    mov r11, 0x202 ; RFLAGS
    o64 sysret