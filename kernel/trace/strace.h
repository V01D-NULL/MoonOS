#ifndef STRACE_H
#define STRACE_H

#include <stdint.h>
#include <stddef.h>

#define UNWIND_MAX 20 /* Unwind up to twenty stack frames */

struct stacktrace {
    size_t address;
};

struct stacktrace_result {
    size_t count;
    size_t caller_rbp;          // rbp of the function that called strace()
    size_t caller_frame_size;   // Size of the callers stackframe
    struct stacktrace trace_results[UNWIND_MAX];
};

struct stacktrace_result backtrace_stack(int frames);
void strace_save_rsp(uint64_t rsp);

#endif // STRACE_H