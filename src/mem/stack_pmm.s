section .text
    extern kprintf
    global s_pmm

msg: dw "s_pmm: ", 0x0A, 0x0D, 0

s_pmm:
    ;mov eax, [addr]
    push msg
    ;push eax
    call kprintf
    ret