bits 64
section .text

%include "asm/defs.inc"

extern syscall_handler
global x86_syscall_handler
x86_syscall_handler:
	swapgs

	mov qword gs:0x16, qword 1
	mov qword gs:0x08, rsp ; Save user rsp
    mov rsp, qword gs:0x0 ; Set syscall handler stack (shared with kernel)

	cld
    pusha64 ; Todo: Let the user do this
	mov rbp, qword 0
    call syscall_handler
    popa64

    mov rsp, qword gs:0x08 ; Restore original rsp
	mov qword gs:0x16, qword 0
    swapgs

    o64 sysret
