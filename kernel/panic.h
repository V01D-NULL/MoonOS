#ifndef PANIC_H
#define PANIC_H

#include <amd64/moon.h>
#include <stdint.h>
#include <trace/strace.h>

// panic if eval evaluates to true
#define panic_if(eval, ...)     \
    {                           \
        if (eval) {             \
            panic(__VA_ARGS__); \
        } else {                \
            (void)0;            \
        }                       \
    }

#define panic(...)                                     \
    {                                                  \
        uint64_t rsp = 0;                              \
        uint64_t rbp = 0;                              \
        __asm__("mov %%rsp, %0"                        \
                : "=r"(rsp));                          \
        __asm__("mov %%rbp, %0"                        \
                : "=r"(rbp));                          \
        _panic(rbp, rsp, __VA_ARGS__);                 \
    }

gnu_no_return _panic(uint64_t rbp, uint64_t rsp, const char* fmt, ...);

#endif // PANIC_H