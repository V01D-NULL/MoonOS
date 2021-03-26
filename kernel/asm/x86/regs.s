global ASM_x86_regs_read_rip

;struc CPU

;endstruc

ASM_x86_regs_read_rip:
    call .get_rip
    
    
    .get_rip:
        pop rax
    
    ret