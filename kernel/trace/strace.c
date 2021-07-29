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

#define FUNC 0xffffffff80203e30

static void walk_frames(int frame_count)
{
	uint64_t rsp;
	struct strace *stk;
	asm volatile("mov %%rbp, %0" : "=r"(stk):: "memory");

	debug(true, "Raw stack trace: \n");
	
	for (int i = 0; i < frame_count && stk; i++)
	{
		// TODO:
		// 1. Write a script that converts the map file into a parsable, stripped down version
		// 2. Import that parsable map into the C kernel (create a dummy file for successful compilation and replace it with the script) or output it into a header file
		// 3. The parsable format has a 2D array of [addr, "func_name"] pairs, now loop through that function and check if stk->rip > 2d_arry[addr] && stk->rip > 2d_array[previous_addr_in_arry] { stk->rip = 2d_array[addr]; }
		
		if (stk->rip > FUNC)
		{
			stk->rip = FUNC;
		}

		debug(true, "<trace+%d> (base) 0x%llx - (top) 0x%llx\n", i, stk->rip, GENERIC_CAST(size_t, stk->rbp));
		stk = stk->rbp;
	}
}