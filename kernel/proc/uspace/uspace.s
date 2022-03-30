bits 64
section .text

%include "asm/defs.inc"

global enter_ring3_sysret
enter_ring3_sysret:
    jmp rdi
    ; swapgs
    ; mov rsp, rsi  ; new rsp
    ; add rsi, 8192 ; 8kib of stack space
    ; mov rbp, rsi

    ; mov rcx, rdi   ; RIP
    ; mov r11, 0x202 ; RFLAGS
    ; o64 sysret