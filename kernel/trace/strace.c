#include "strace.h"
#include <stdint.h>
#include <stdbool.h>
#include "../drivers/io/serial.h"
#include "../util/ptr.h"

struct strace
{
	struct strace *rbp;
	uint64_t *rip;
};

void strace()
{
	walk_frames(10);
}

void walk_frames(int frame_count)
{
	struct strace *stk;
	__asm__ volatile("mov %%rbp, %0" : "=r"(stk):: "memory");

	debug(true, "Raw stack trace: \n");
	
	for (int i = 0; i < frame_count && stk; i++)
	{
		debug(true, "<trace+%d> (base) 0x%llx - (top) 0x%llx\n", i, stk->rip, GENERIC_CAST(uintptr_t, stk->rbp));
		stk = stk->rbp;
	}
}