global ASM_x86_cpuid_vendor_string
global ASM_x86_cpuid_check_bi_local_apic

extern x86_cpuid_vendor_string

ASM_x86_cpuid_vendor_string:
    xor rax, rax
    cpuid

    mov rdi, rbx
    mov rsi, rdx
    mov rdx, rcx

    call x86_cpuid_vendor_string

    ret

; Bit 9 of edx checks if there is a built-in local apic
ASM_x86_cpuid_check_bi_local_apic:
    mov eax, 0x01
    cpuid
    mov rax, rdx
    ret