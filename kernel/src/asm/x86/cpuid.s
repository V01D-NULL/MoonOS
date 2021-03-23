global ASM_x86_cpuid_vendor_string
global ASM_x86_cpuid_check_apic

ASM_x86_cpuid_vendor_string:
    xor rax, rax
    cpuid

    mov [ rax ], rbx
    mov [rax+4], rdx
    mov [rax+8], rcx
    ret

ASM_x86_cpuid_check_apic:
    mov rax, 0x01
    cpuid
    mov [rax], rdx
    ret