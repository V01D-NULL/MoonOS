#include "stack_trace.h"

struct stacktrace_result arch_trace_stack(int frames)
{
    if (frames > UNWIND_MAX)
        frames = UNWIND_MAX;

    struct stacktrace_result result;
    void                   **rbp;
    asm volatile("mov %%rbp, %0" : "=r"(rbp));

    int i = 0;
    for (; i < frames && rbp != NULL; i++)
    {
        void *return_address    = *(rbp + sizeof(int8_t));
        result.trace_results[i] = (size_t)return_address;
        rbp                     = (void **)(*rbp);
    }

    // Note: Setting result.count = frames could cause UB as the stack
    // is not guaranteed to be comprised of `frames` stack frames.
    result.count = i - 1;

    // Save caller rbp to dump stack contents later. Since
    // panic() is included in the stack trace, we use at
    // index 1 instead of 0.
    result.caller_rbp = result.trace_results[1];

    return result;
}
