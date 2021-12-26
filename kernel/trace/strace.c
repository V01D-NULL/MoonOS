#include "strace.h"
#include <stdbool.h>
#include <devices/serial/serial.h>
#include <ds/linked_list.h>
#include "sym.h"

struct stackframe {
    struct stackframe *frame;
    uint64_t rip;
};

// The function about to call strace() or panic() should preserve
// the stackpointer in order to correctly dump the stacks contents
static uint64_t faulty_rsp = 0;
void strace_save_rsp(uint64_t rsp) {
    faulty_rsp = rsp;
}

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
        stackframe = stackframe->next;
    }

    //Note: Setting result.count = frames could cause UB as the stack
    //is not guaranteed to be comprised of `frames` stack frames.
    result.count = i - 1;
    result.caller_rbp = result.trace_results[1].address; // Save caller rbp to dump stack contents later. Since panic() is
                                                         // included in the stack trace, we use at index 1 instead of 0.
    result.caller_frame_size = faulty_rsp - result.caller_rbp;
    
    return result;
}
