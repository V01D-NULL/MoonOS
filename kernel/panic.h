#ifndef PANIC_H
#define PANIC_H

#include <moon.h>
#include <arch.h>
#include <trace/stack_trace.h>

// panic if eval evaluates to true
#define panic_if(eval, ...)     \
	do                          \
	{                           \
		if (eval)               \
		{                       \
			panic(__VA_ARGS__); \
		}                       \
	} while (0)

#define panic(...)                                                                \
	do                                                                            \
	{                                                                             \
		extern NORETURN void __panic(size_t bp, size_t sp, string_view fmt, ...); \
		size_t sp = arch_get_sp();                                                \
		size_t bp = arch_get_bp();                                                \
		__panic(bp, sp, __VA_ARGS__);                                             \
	} while (0)

#endif // PANIC_H