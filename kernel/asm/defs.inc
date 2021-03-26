[bits 64]

global _load_bin

%define KRNL_CS 0x8
%define KRNL_DS 0x10

%macro pusha64 0
    push rax
    push rbx
    push rcx
    push rdx
    push rbp
    push rdi
    push rsi
    push r8
    push r9
    push r10
    push r11
    push r12
    push r13
    push r14
    push r15
%endmacro

%macro popa64 0
    pop r15
    pop r14
    pop r13
    pop r12
    pop r11
    pop r10
    pop r9
    pop r8
    pop rsi
    pop rdi
    pop rbp
    pop rdx
    pop rcx
    pop rbx
    pop rax
%endmacro

%macro _cleanup_stack 1
    add rsp, %1
%endmacro


;
; USAGE:
;
; asm:
;   _load_bin reference_for_C_code, "filename.bmp/tga/txt/whatever"
; C:
; extern const char reference_for_C_code[];
; printf(reference_for_C_code);
;
%macro _load_bin 2
    section .rodata
    global %1
%1:
    incbin %2
    db 0
    sizeof_%1: db sizeof_%1 - %1
%endmacro