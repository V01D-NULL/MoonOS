#include <msr.h>
#include <int/gdt.h>
#include <moon-extra/compiler.h>
#include <panic.h>
#include <printk.h>
#include <abi/syscalls.h>
#include <sys/context_switch.h>

extern void x86_syscall_handler(void);

void arch_init_syscall(void)
{
	wrmsr(IA32_EFER, rdmsr(IA32_EFER) | (1 << 0)); // Enable MSR IA32_EFER.SCE (System call extensions)
	wrmsr(IA32_FMASK, (1 << 9) | (1 << 8));		   // Mask IF, TF
	wrmsr(STAR, (KRNL_CS64 << 32) | (KRNL_DS64 | 3) << 48);
	wrmsr(LSTAR, (uint64_t)&x86_syscall_handler);
}

void syscall_handler(GeneralRegisters reg)
{
	
	// debug(true, "\nSyscall handler\n");
	// debug(false, "{\n"
	// 			 "   arg0: 0x%lx | arg1: 0x%lx | arg2: 0x%lx\n"
	// 			 "   arg3: 0x%lx | arg4: 0x%lx | arg5: 0x%lx\n"
	// 			 "   arg6: 0x%lx\n"
	// 			 "}\n",
	// 	  reg.rax, reg.rdi, reg.rsi,
	// 	  reg.rdx, reg.r10, reg.r8, reg.r9);

	// if (reg.rax > ARRAY_SIZE(syscall_list))
	// 	panic("%ld is out of range", reg.rax);

	auto status = syscall_list[reg.rax](reg.rdi, reg.rsi, reg.rdx, reg.r10, reg.r8, reg.r9);

	if (status == -1)
		TODO("syscall(%d) is a stub!", reg.rax);
}