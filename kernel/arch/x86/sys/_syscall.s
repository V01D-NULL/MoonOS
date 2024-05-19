; bits 64
; section .text

; %include "asm/defs.inc"

; extern syscall_handler
; global x86_syscall_handler
; x86_syscall_handler:
; 	; swapgs

; 	; mov qword gs:0x16, qword 1
; 	; mov qword gs:0x08, rsp ; Save user rsp
;     ; mov rsp, qword gs:0x0 ; Set syscall handler stack (shared with kernel)

; 	; cld
;     pusha64 ; Todo: Let the user do this
; 	; mov rbp, qword 0
;     call syscall_handler
;     popa64

;     ; mov rsp, qword gs:0x08 ; Restore original rsp
; 	; mov qword gs:0x16, qword 0
;     ; swapgs

;     o64 sysret


bits 64
section .text

%include "asm/defs.inc"

extern syscall_handler
global x86_syscall_handler

x86_syscall_handler:
    swapgs                        ; Switch to kernel stack segment
    mov qword [gs:0x08], rsp      ; Save user rsp in kernel's per-CPU storage
    mov rsp, qword [gs:0x0]       ; Load kernel rsp from per-CPU storage

    cli

    pusha64                       ; Push all general-purpose registers
    mov rbp, qword 0              ; Clear rbp (optional, but common practice)
    call syscall_handler          ; Call the C syscall handler
    popa64                        ; Pop all general-purpose registers

    mov rsp, qword [gs:0x08]      ; Restore user rsp
    swapgs                        ; Switch back to user stack segment

    o64 sysret                       ; Return to user mode
