bits 64
section .text

%include "asm/defs.inc"

global enter_ring3_sysret
enter_ring3_sysret:
    ; Setup user stack..
    add rsi, 8192
    mov rbp, rsi
    mov rsp, rbp

    ; Enter ring3
    mov rcx, rdi   ; RIP
    mov r11, 0x202 ; RFLAGS
    o64 sysret