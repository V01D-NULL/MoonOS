#include <abi/syscalls.h>

extern void do_sys(void);

void main(void)
{
	do_sys();
	// syscall_log("init: Started init daemon\n", 27);
	
// 	for (int i = 0; i < 40; i++)
// 		syscall_log("FGHIJ\n", 6);

	for (;;)
	{
		do_sys();
	}
}