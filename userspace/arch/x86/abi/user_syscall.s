bits 64

global _syscall
_syscall:        
    ; push rcx
    ; push r11 

	; mov rax, rdi
	; mov rdx, rsi

    
    ; mov r10, %4
    ; mov r8,  %5
    ; mov r9,  %6
    ; syscall
    ; pop r11
    ; pop rcx

    ; ; ".att_syntax prefix\n"
    ;     :
    ;     : "a"(selector), "D"(arg0), "S"(arg1), "d"(arg2), "r"(arg3), "r"(arg4), "r"(arg5)
    ;     : "memory", "r8", "r9", "r10"
