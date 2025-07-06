bits 64
section .text

%include "asm/defs.inc"

extern syscall_handler
global x86_syscall_handler
x86_syscall_handler:
    cli
	swapgs

	mov qword gs:0x16, qword 1
	mov qword gs:0x08, rsp ; Save user rsp
    mov rsp, qword gs:0x0 ; Set syscall handler stack (shared with kernel)

    pusha64 ; Todo: Let the user do this
	mov rbp, qword 0
    call syscall_handler
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    ; rax contains the status code - don't pop it.

    mov rsp, qword gs:0x08 ; Restore original rsp
	mov qword gs:0x16, qword 0
    swapgs

    sti
    o64 sysret
