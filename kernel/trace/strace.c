#include "strace.h"
#include <stdbool.h>
#include <drivers/vbe/vbe.h>
#include <drivers/io/serial.h>
#include <ds/linked_list.h>

#include "sym.h"
struct stackframe {
    struct stackframe *frame;
    uint64_t rip;
};
struct stacktrace_result backtrace_stack(int frames)
{
    if (frames > UNWIND_MAX)
    {
        frames = UNWIND_MAX;
    }

    struct stacktrace_result result;
    struct ds_default_list *stackframe;
    __asm__("mov %%rbp, %0"
            : "=r"(stackframe));

    int i = 0;
    while (i < frames && stackframe)
    {
        result.trace_results[i++].address = stackframe->value;
        ds_default_next(&stackframe);
    }

    //Note: Setting result.count = frames could cause UB as the stack
    //is not guaranteed to be comprised of `frames` stack frames.
    result.count = i - 1;

    return result;
}
