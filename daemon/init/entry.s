; global _start

; _start:
; 	mov rax, 1
; 	syscall
; 	jmp _start

global _start

extern main

_start:
	; mov rax, 1
	; mov rdi, 0
	; lea rsi, str
	; syscall
	call main
	jmp $


global do_sys
do_sys:
	mov rax, 1
	mov rdi, 0x1
	mov rsi, str
	mov rdx, 4
	mov r9, 0x1234
	syscall
	ret

str: db 'hi', 0xa, 0