#include <abi/syscalls.h>

void _start(void)
{
	syscall_log("hello: Started hello daemon\n", 29);

	for (;;)
	{
		// syscall_log("ABCDE\n", 7);
		// syscall_log("FGHIJ\n", 7);
	}
}