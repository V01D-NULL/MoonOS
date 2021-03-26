global ASM_x86_cpuid_vendor_string
global ASM_x86_cpuid_check_bi_local_apic

extern x86_cpuid_vendor_string

ASM_x86_cpuid_vendor_string:
    push rbx ; The sysv ABI expects rbx to not be clobbered which is the case with cpuid
    xor rax, rax
    cpuid

    mov rdi, rbx
    mov rsi, rdx
    mov rdx, rcx

    call x86_cpuid_vendor_string
    pop rbx
    ret

