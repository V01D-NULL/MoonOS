global _start

extern main

_start:
	; mov rax, 1
	; mov rdi, 0x1
	; mov rsi, str
	; mov rdx, 4
	; syscall
	call main
	jmp $

; str: db 'hey', 0xa, 0