bits 64
section .text

%include "asm/defs.inc"

extern syscall_handler
global x86_syscall_handler
x86_syscall_handler:
	swapgs
	
	mov qword [gs:0x08], rsp ; Save user rsp
    mov rbp, qword [gs:0x0]  ; Set syscall handler stack (shared with kernel)
	; jmp $
    ; mov rbp, qword 0
	sti

    pusha64 ; Todo: Let the user do this
    call syscall_handler
    popa64
	
	cli
    mov rsp, qword [gs:0x08] ; Restore original rsp
	mov rbp, qword 0
    swapgs
    
	mov rax, 0
    o64 sysret
