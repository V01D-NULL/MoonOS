bits 64
section .text

%include "asm/defs.inc"

extern syscall_handler
global x86_syscall_handler
x86_syscall_handler:
    swapgs
    
    mov [gs:0x08], rsp ; Save user rsp
    mov rsp, [gs:0x0]  ; Set syscall handler stack (shared with kernel)
    mov rbp, 0

    push r11
    push rcx
    pusha64
    call syscall_handler
    popa64
    
    mov rsp, [gs:0x08] ; Restore original rsp
    swapgs
    
    o64 sysret
