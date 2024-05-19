#include <abi/syscalls.h>

void main(void)
{
	for(;;)
	syscall_log("init\n", 5);
}