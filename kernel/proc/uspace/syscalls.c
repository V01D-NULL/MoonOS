#include "syscalls.h"
#include <amd64/msr.h>
#include <printk.h>

extern void x86_syscall_handler(void);
static bool supports_syscall = true;

// Does this computer support syscall/sysret or do we need to use the slower int/iretq?
bool fast_syscall_supported(void) { return supports_syscall; }

void init_syscalls(void)
{
    if (!cpu_has_msr())
    {
        supports_syscall = false;
        printk("syscalls", "\033[93mWARNING:\033[39m The `syscall` instruction is not supported, syscalls may be significantly slower if done via a syscall instruction\n"); // Moon traps #UD exceptions and will use int/iretq instead which is a lot slower.
        return;
    }

    wrmsr(IA32_EFER, rdmsr(IA32_EFER) | (1 << 0)); // Enable MSR IA32_EFER.SCE (System call extensions)

    uint64_t star = (0x28UL << 32) | (0x30UL << 48); // Kernel CS: 0x28, Kernel DS: 0x30
    wrmsr(STAR, star);

    wrmsr(LSTAR, (uint64_t)&x86_syscall_handler);
}

void syscall_handler(uint64_t current_stack)
{
    debug(true, "Syscall handler (stack: %llX)\n", current_stack);
    for(;;);
}