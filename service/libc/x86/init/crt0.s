global _start
extern libc_entry

_start:
	mov rdi, libc_entry
    ; rsi = argc
    ; rdx = argv
	jmp rdi