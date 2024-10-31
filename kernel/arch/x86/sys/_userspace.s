bits 64
section .text

%include "asm/defs.inc"

global arch_enter_userspace
arch_enter_userspace:
    ; Clear all registers except for RDI, RSI, and RBP because they are
    ; used for arguments and do not contain kernel addresses or data
    xor rax, rax
    xor rbx, rbx
    xor rcx, rcx
    xor rdx, rdx
    xor r8, r8
    xor r9, r9
    xor r10, r10
    xor r11, r11
    xor r12, r12
    xor r13, r13
    xor r14, r14
    xor r15, r15

    swapgs

    ; Setup user stack..
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


extern timer_irq
global task_switch_handler
task_switch_handler:
    cld
    pusha64
    mov rdi, rsp
    call timer_irq
    popa64
    add rsp, 16
    iretq
