#include "stack_trace.h"

struct stackframe_list {
	struct stackframe_list *next;
	size_t value;
};

struct stacktrace_result arch_trace_stack(int frames)
{
    if (frames > UNWIND_MAX)
    {
        frames = UNWIND_MAX;
    }

    struct stacktrace_result result;
    struct stackframe_list *stackframe;
    asm volatile("mov %%rbp, %0"
            : "=r"(stackframe) :: "memory");

    int i = 0;
    while (i < frames && stackframe)
    {
		result.trace_results[i++].address = stackframe->value;
        stackframe = stackframe->next;
    }

    //Note: Setting result.count = frames could cause UB as the stack
    //is not guaranteed to be comprised of `frames` stack frames.
    result.count = i - 1;
    result.caller_rbp = result.trace_results[1].address; // Save caller rbp to dump stack contents later. Since panic() is
                                                         // included in the stack trace, we use at index 1 instead of 0.
    
    return result;
}
