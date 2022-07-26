#ifndef MOON_DEFS_H
#define MOON_DEFS_H

#include <stddef.h>

#define NORETURN __attribute__((noreturn))
#define ALIGN_SECTION(x)  __attribute__((aligned (x)))
#define PACKED __attribute__((packed))
#define UNUSED __attribute__((unused))

#define STATIC_INLINE __attribute__((always_inline)) static inline
#define EXTERNAL(linker_symbol) extern size_t linker_symbol[];
#define SECTION(_section) __attribute__((section(_section)))
#define MAYBE_UNUSED __attribute__((maybe_unused))
#define USED __attribute__((used))

#endif // MOON_DEFS_H
