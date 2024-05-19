bits 64
section .text

%include "asm/defs.inc"

global arch_enter_userspace
arch_enter_userspace:
	; Swap out the kernel gs
	swapgs

    ; Setup user stack..
    mov rbp, rsi
    mov rsp, rbp

    ; NOTE: SS & CS are set by sysret
    ; Set user segment selectors
    mov ax, 0x1B
    mov ds, ax
    mov fs, ax
    mov gs, ax
    mov es, ax
	
    ; Enter ring3
    mov rcx, rdi   ; RIP
	mov r11, 0x202 ; RFLAGS
    o64 sysret


extern timer_irq
global task_switch_handler
task_switch_handler:
    cld
    pusha64
    mov rdi, rsp
    call timer_irq
    popa64
    add rsp, 16
    iretq

global switch_to_task
switch_to_task:
    ; Load the new task's stack pointer
    mov rsp, [rdi]
    
    ret
    ; ; Restore the context from the new stack
    ; popa64

    ; ; Restore RIP, CS, RFLAGS, RSP, and SS
    ; add rsp, 8  ; Skip saved RSP (already loaded)
    ; pop rax     ; Load new RFLAGS
    ; pop rbx     ; Load new CS
    ; pop rcx     ; Load new RIP
    ; mov ds, rbx ; Load new data segment
    ; mov es, rbx ; Load new extra segment

    ; ; Load new stack segment and stack pointer
    ; swapgs
    ; add rsp, 16
    ; mov ss, rbx
    ; mov rsp, rax

    ; ; Return to the new task
    ; iretq

