bits 64
section .text

extern syscall_handler
global x86_syscall_handler
x86_syscall_handler:
    swapgs
    push rcx ; Don't clobber rcx- It holds return address.
    mov rdi, [gs:0x0]
    call syscall_handler
    pop rcx
    swapgs
    o64 sysret