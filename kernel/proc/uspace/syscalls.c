#include "syscalls.h"
#include <moon-extra/compiler.h>
#include <cpu.h>
#include <printk.h>
#include <base/base-types.h>
#include <panic.h>
#include <interrupts.h>

// TODO:
// arch/<arch>/sys/syscall.c will unpack the arguments and simply call one of these functions.
// Probably best if it's done by 'externing' the syscall_list array.

// #define Type uint64_t
// #define ignore Type UNIQUE_NAME(ignore)

// typedef int (*syscall_routine)(Type, Type, Type, Type, Type, Type);

// // Syscall stub
// int sys_stub(ignore, ignore, ignore, ignore, ignore, ignore) { return -1; }

// int sys_log(Type fd, Type buff, Type len, ignore, ignore, ignore)
// {
// 	(void)fd;

// 	string_view ptr = (string_view)buff;
// 	if (!ptr)
// 		return -1;
	
// 	for (auto i = 0; i < len; i++)
// 		puts("%c", ptr[i]);

// 	return 0;
// }

// syscall_routine syscall_list[] = {
// 	sys_stub,
// 	sys_log,
// };

// void syscall_handler(general_registers reg)
// {
// 	// debug(true, "\nSyscall handler\n");
// 	// debug(false, "{\n"
// 	// 			 "   arg0: 0x%p | arg1: 0x%p | arg2: 0x%p\n"
// 	// 			 "   arg3: 0x%p | arg4: 0x%p | arg5: 0x%p\n"
// 	// 			 "   arg6: 0x%p\n"
// 	// 			 "}\n",
// 	// 	  reg.rax, reg.rdi, reg.rsi,
// 	// 	  reg.rdx, reg.r10, reg.r8, reg.r9);

// 	if (reg.rax > ARRAY_SIZE(syscall_list))
// 		panic("%ld is out of range", reg.rax);

// 	auto status = syscall_list[reg.rax](reg.rdi, reg.rsi, reg.rdx, reg.r10, reg.r8, reg.r9);

// 	if (status == -1)
// 		TODO("syscall(%d) is a stub!", reg.rax);
// }