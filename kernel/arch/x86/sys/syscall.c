#include <msr.h>
#include <int/gdt.h>
#include <moon-extra/compiler.h>
#include <panic.h>
#include <printk.h>

extern void x86_syscall_handler(void);

void arch_init_syscall(void)
{
	wrmsr(IA32_EFER, rdmsr(IA32_EFER) | (1 << 0)); // Enable MSR IA32_EFER.SCE (System call extensions)
	wrmsr(IA32_FMASK, (1 << 9) | (1 << 8));		   // Mask IF, TF
	wrmsr(STAR, (KRNL_CS64 << 32) | (KRNL_DS64 | 3) << 48);
	wrmsr(LSTAR, (uint64_t)&x86_syscall_handler);
}

#define Type uint64_t
#define ignore Type UNIQUE_NAME(ignore)

typedef int (*syscall_routine)(Type, Type, Type, Type, Type, Type);

// Syscall stub
int sys_stub(ignore, ignore, ignore, ignore, ignore, ignore) { return -1; }

int sys_log(Type fd, Type buff, Type len, ignore, ignore, ignore)
{
	(void)fd;

	string_view ptr = (string_view)buff;
	if (!ptr)
		return -1;
	
	for (auto i = 0; i < len; i++)
		puts("%c", ptr[i]);

	return 0;
}

syscall_routine syscall_list[] = {
	sys_stub,
	sys_log,
};

void syscall_handler(general_registers reg)
{
	// debug(true, "\nSyscall handler\n");
	// debug(false, "{\n"
	// 			 "   arg0: 0x%p | arg1: 0x%p | arg2: 0x%p\n"
	// 			 "   arg3: 0x%p | arg4: 0x%p | arg5: 0x%p\n"
	// 			 "   arg6: 0x%p\n"
	// 			 "}\n",
	// 	  reg.rax, reg.rdi, reg.rsi,
	// 	  reg.rdx, reg.r10, reg.r8, reg.r9);

	if (reg.rax > ARRAY_SIZE(syscall_list))
		panic("%ld is out of range", reg.rax);

	auto status = syscall_list[reg.rax](reg.rdi, reg.rsi, reg.rdx, reg.r10, reg.r8, reg.r9);

	if (status == -1)
		TODO("syscall(%d) is a stub!", reg.rax);
}