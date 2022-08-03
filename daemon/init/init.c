#include <abi/syscalls.h>

void _start(void)
{
	syscall_log("init: Started init daemon\n", 27);

	for (;;)
	{
		syscall_log("ABCDE\n", 7);
		syscall_log("FGHIJ\n", 7);
	}
}