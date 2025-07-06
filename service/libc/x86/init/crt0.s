global _start
extern libc_entry
extern main

_start:
    mov rdi, main
    mov rsi, [rsp]      ; argc
    mov rdx, [rsp+8]    ; argv

    jmp libc_entry
