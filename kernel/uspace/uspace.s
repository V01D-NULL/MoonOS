bits 64
section .text

%include "asm/defs.inc"

global jump_to_user_address
jump_to_user_address:
    mov ax, (7 * 8) | 3 ; ring 3 data with bottom 2 bits set for ring 3
	mov ds, ax
	mov es, ax 
	mov fs, ax 
	mov gs, ax ; SS is handled by iret
 
	; set up the stack frame iret expects
    pusha64
	mov rax, rsp
	push (7 * 8) | 3 ; data selector
	push rax ; current esp
	pushf ; eflags
	push (8 * 8) | 3 ; code selector (ring 3 code with bottom 2 bits set for ring 3)
	push rdi ; New RIP
    iretq