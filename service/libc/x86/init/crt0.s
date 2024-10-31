global _start
extern libc_entry
extern main

_start:
    mov rdi, main
    ; TODO: Pass argc and argv
	jmp libc_entry
