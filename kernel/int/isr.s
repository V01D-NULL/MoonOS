%include "asm/defs.inc"

extern isr_handler
extern idt_set_entry

_asm_isr_handler_stub:
    cld
    pusha64
    
    call isr_handler   ; C interrupt handler routine
    popa64
    _cleanup_stack 16  ; Clean up pushed error code and interrupt number from stack
    iretq              ; Pop other flags and return to normal execution State


; params: r10 = isr{number} aka `offset`, r11 = index
zero_user_isr:
    mov rdx, 0x8E   ; type_attr
    mov rcx, r10    ; offset
    mov r8, r11     ; idx
    call idt_set_entry
    ret

global load_idt
load_idt:
    ; Set isrs 48-255 to dummy entries, otherwise it's just a nasty #GP
    push rdi
    mov rdi, 0x08 ; Selector
    mov rsi, 0x0  ; ist
    %assign i 48
    %rep 208
    mov r10, isr%+i
    mov r11, i
    call zero_user_isr
    %assign i i+1
    %endrep
    pop rdi

    lidt [rdi]

    ; Reload cs and ds
    push 0x28
    push .reload
    retfq
    .reload:
       mov ax, 0x30
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

; IRQ's
%assign n 32
%rep 16
    isr n
%assign n n+1
%endrep

; User defined isrs
%assign n 48
%rep 208
    isr n
%assign n n+1
%endrep
