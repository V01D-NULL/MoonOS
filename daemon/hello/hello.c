#include <abi/syscalls.h>

extern void do_sys(void);

void main(void)
{
	// syscall_log("hello: Started hello daemon\n", 29);
	do_sys();
	
	for (;;)
	{
		do_sys();
		// syscall_log("test\n", 5);
	}
}