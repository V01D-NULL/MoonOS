bits 64
section .text

%include "asm/defs.inc"

extern syscall_handler
global x86_syscall_handler
x86_syscall_handler:
    swapgs
    pusha64
    
    mov rdi, [gs:0x0]
    call syscall_handler
    
    popa64
    swapgs
    o64 sysret
