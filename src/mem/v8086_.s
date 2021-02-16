global enter_v8086

enter_v8086:
    mov ebp, esp ; Save stack pointer

    push DWORD [ebp+4]  ; SS
    push DWORD [ebp+8]  ; ESP

    pushfd              ; eflags
    or DWORD [esp], (1 << 17) ; Change the 17th bit of the eflags register. The [] tell nasm to change the contents of the register i.e. the bit is changed for real
    push DWORD [ebp+12] ; CS
    push DWORD [ebp+16] ; EIP
    iret
    