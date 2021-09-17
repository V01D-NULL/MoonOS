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
    struct stacktrace trace_results[UNWIND_MAX];
};

struct stacktrace_result backtrace_stack(int frames);

#endif // STRACE_H