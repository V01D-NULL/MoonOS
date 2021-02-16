section .text
    global find_pci_bus

find_pci_bus:
    mov eax, 0xCFC
    in eax, dx
    cmp edx, " ICP"
    jz .fail

    mov eax, 0 ; return value

    ret

.fail:
    mov eax, -1 ; return value
    ret
