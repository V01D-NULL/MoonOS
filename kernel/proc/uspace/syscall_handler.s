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

    ; NOTE: I'm not setting SS or CS as these are automatically set by sysret
    ; Set user segment selectors
    mov ax, 0x3B
    mov ds, ax
    mov fs, ax
    mov gs, ax
    mov es, ax

    o64 sysret
