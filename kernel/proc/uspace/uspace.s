bits 64
section .text

%include "asm/defs.inc"

global jump_to_user_address

; rdi = entry point
; rsi = rsp (stack size is fixed to 8kib for now)
jump_to_user_address:
    mov ax, (7 * 8) | 3 ; ring 3 data with bottom 2 bits set for ring 3
	mov ds, ax
	mov es, ax 
	mov fs, ax 
	mov gs, ax ; SS is handled by iret

	
    ; mov rax, rsi ; User stack
    pusha64
	push (7 * 8) | 3 ; data selector

    ; Setup bp & sp
    mov rbx, rsi
    add rbx, 8192 ; 8kib of stack space
	push rsi  ; new rsp
    mov rbp, rbx
    
	pushf ; rflags
	push (8 * 8) | 3 ; code selector (ring 3 code with bottom 2 bits set for ring 3)
	push rdi ; New RIP
    iretq