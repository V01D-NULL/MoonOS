#ifndef PANIC_H
#define PANIC_H

#include <amd64/moon.h>
#include <stdint.h>
#include <trace/strace.h>

// panic if eval evaluates to true
#define panic_if(eval, ...)     \
    do                          \
    {                           \
        if (eval)               \
        {                       \
            panic(__VA_ARGS__); \
        }                       \
        else                    \
        {                       \
            (void)0;            \
        }                       \
    } while (0)

#define panic(...)                     \
    do                                 \
    {                                  \
        uint64_t rsp = 0;              \
        uint64_t rbp = 0;              \
        asm("mov %%rsp, %0"            \
            : "=r"(rsp));              \
        asm("mov %%rbp, %0"            \
            : "=r"(rbp));              \
        _panic(rbp, rsp, __VA_ARGS__); \
    } while (0)

NORETURN void _panic(uint64_t rbp, uint64_t rsp, const char *fmt, ...);

#endif // PANIC_H