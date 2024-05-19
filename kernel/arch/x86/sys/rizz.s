; extern current_task
; extern next_task

; section .data

; section .text
; global context_switch

; context_switch:
;     ; Save current task context
;     mov rax, [current_task]
;     lea rbx, [rax + 40]             ; Offset for `struct arch_task_registers` inside `Task`
;     mov [rbx + 0], r15
;     mov [rbx + 8], r14
;     mov [rbx + 16], r13
;     mov [rbx + 24], r12
;     mov [rbx + 32], r11
;     mov [rbx + 40], r10
;     mov [rbx + 48], r9
;     mov [rbx + 56], r8
;     mov [rbx + 64], rsi
;     mov [rbx + 72], rdi
;     mov [rbx + 80], rbp
;     mov [rbx + 88], rdx
;     mov [rbx + 96], rcx
;     mov [rbx + 104], rbx
;     mov [rbx + 112], rax
;     pushfq
;     pop qword [rbx + 128]          ; Save flags

;     ; Save instruction pointer
;     lea rdx, [rbx + 120]
;     call save_rip

; save_rip:
;     pop qword [rdx]

;     ; Switch to next task
;     mov rcx, [next_task]
;     lea rdx, [rcx + 40]            ; Offset for `struct arch_task_registers` inside `Task`
;     mov r15, [rdx + 0]
;     mov r14, [rdx + 8]
;     mov r13, [rdx + 16]
;     mov r12, [rdx + 24]
;     mov r11, [rdx + 32]
;     mov r10, [rdx + 40]
;     mov r9, [rdx + 48]
;     mov r8, [rdx + 56]
;     mov rsi, [rdx + 64]
;     mov rdi, [rdx + 72]
;     mov rbp, [rdx + 80]
;     mov rdx, [rdx + 88]
;     mov rcx, [rdx + 96]
;     mov rbx, [rdx + 104]
;     mov rax, [rdx + 112]

;     ; Load instruction pointer
;     push qword [rdx + 120]
;     ret

;     ; Load flags
;     push qword [rdx + 128]
;     popfq

;     ; Update current task pointer
;     mov [current_task], rcx

;     ret
