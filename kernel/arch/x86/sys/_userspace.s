bits 64
section .text

%include "asm/defs.inc"

global arch_enter_userspace
arch_enter_userspace:
	; Swap out the kernel gs
	swapgs

    ; Setup user stack..
    add rsi, 8192
    mov rbp, rsi
    mov rsp, rbp

    ; NOTE: SS & CS are set by sysret
    ; Set user segment selectors
    mov ax, 0x1B
    mov ds, ax
    mov fs, ax
    mov gs, ax
    mov es, ax

    ; Enter ring3
    mov rcx, rdi   ; RIP
	mov r11, 0x202 ; RFLAGS
    o64 sysret
