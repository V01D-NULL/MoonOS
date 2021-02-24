section .text
    global enter_usermode

%define USERMODE_DS 0x23
%define USERMODE_CS 0x1B

enter_usermode:
    cli
    mov ax, USERMODE_DS
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov eax, esp
    
    push USERMODE_DS
    push eax
    pushf
    push USERMODE_CS
    push .dummy
    iret
.dummy: