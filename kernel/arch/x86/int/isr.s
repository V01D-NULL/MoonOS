%include "asm/defs.inc"

extern isr_handler
extern idt_set_entry

%macro isr_swapgs 1
	cmp [rsp+24], dword 8 ; Compare kernel CS with the CS on the interrupt stack

	je .%1
	swapgs
	.%1:
%endmacro

_asm_isr_handler_stub:
    cld
	isr_swapgs swapgs_entry
    pusha64
    mov rdi, rsp
    call isr_handler   ; C interrupt handler routine
    popa64
	isr_swapgs swapgs_exit
    add rsp, 16 	 ; Clean up pushed error code and interrupt number from stack
    iretq            ; Pop other flags and return to normal execution State

global load_idt
load_idt:
    lidt [rdi]

    push 0x08
    lea rax, [rel .reload]
    push rax
    retfq
    .reload:
       mov ax, 0x10
       mov ds, ax
       mov fs, ax
       mov gs, ax
       mov ss, ax
       mov es, ax
       sti
       ret

%macro isr 1
global isr%1
isr%1:
    push 0  ; Dummy error code
    push %1 ; Push interrupt number
    jmp _asm_isr_handler_stub
%endmacro

%macro isr_err 1
global isr_err%1
isr_err%1:
    ; Error code pushed by CPU
    push %1 ; Push interrupt number
    jmp _asm_isr_handler_stub
%endmacro

; CPU exceptions
%assign n 0
%rep 8
    isr n
%assign n n+1
%endrep

isr_err 8
isr 9
isr_err 10
isr_err 11
isr_err 12
isr_err 13
isr_err 14

%assign n 15
%rep 17
    isr n
%assign n n+1
%endrep

; ; Timer/task switch
global isr32
extern task_switch_handler
isr32:
    push 0  ; Dummy error code
    push 32 ; Push interrupt number
    jmp task_switch_handler

; IRQ's
%assign n 33
%rep 15
    isr n
%assign n n+1
%endrep

; User defined isrs
%assign n 48
%rep 208
   isr n
%assign n n+1
%endrep
