#include "syscalls.h"
#include <util/compiler.h>
#include <amd64/msr.h>
#include <int/gdt.h>
#include <printk.h>
#include <ktypes.h>

#define Type uint64_t
#define ignore Type UNIQUE_NAME(ignore)

extern void x86_syscall_handler(void);

void init_syscalls(void)
{
    wrmsr(IA32_EFER, rdmsr(IA32_EFER) | (1 << 0)); // Enable MSR IA32_EFER.SCE (System call extensions)
    wrmsr(STAR, (KRNL_CS64 << 32) | (KRNL_DS64 | 3) << 48);
    wrmsr(LSTAR, (uint64_t)&x86_syscall_handler);
}

typedef int (*syscall_routine)(Type, Type, Type, Type, Type, Type);

// Syscall stub
int sys_stub(ignore, ignore, ignore, ignore, ignore, ignore) { return -1; }

int sys_log(Type fd, Type buff, Type len, ignore, ignore, ignore)
{
    (void)fd;

    const char *ptr = (const char *)buff;
    for (auto i = 0; i < len; i++)
        fmt_puts("%c", ptr[i]);

    return 0;
}

syscall_routine syscall_list[] = {
    sys_stub,
    sys_log,
};

void syscall_handler(gp_registers_t reg)
{
    debug(true, "\nSyscall handler\n");
    debug(false, "{\n"
                 "   arg0: 0x%p | arg1: 0x%p | arg2: 0x%p\n"
                 "   arg3: 0x%p | arg4: 0x%p | arg5: 0x%p\n"
                 "   arg6: 0x%p\n"
                 "}\n",
          reg.rax, reg.rdi, reg.rsi,
          reg.rdx, reg.r10, reg.r8, reg.r9);

    auto status = syscall_list[reg.rax](reg.rdi, reg.rsi, reg.rdx, reg.r10, reg.r8, reg.r9);
    if (status == -1)
        TODO("syscall(%d) is a stub!", reg.rax);
}