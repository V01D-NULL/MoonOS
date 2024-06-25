#ifndef ARCH_STACKTRACE_H
#define ARCH_STACKTRACE_H

#include <base/base-types.h>

#define UNWIND_MAX 20 /* Unwind up to twenty stack frames */

struct stacktrace_result
{
    size_t   count;
    size_t   caller_rbp;         // rbp of the function that called strace()
    size_t   caller_frame_size;  // Size of the callers stackframe
    uint64_t trace_results[UNWIND_MAX];
};

struct stacktrace_result arch_trace_stack(int frames);

#endif  // ARCH_STACKTRACE_H