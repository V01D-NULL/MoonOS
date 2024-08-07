#ifndef MOON_DEFS_H
#define MOON_DEFS_H

#include <base/base-types.h>

#define NORETURN __attribute__((noreturn))
#define ALIGN_SECTION(x) __attribute__((aligned(x)))
#define PACKED __attribute__((packed))
#define UNUSED __attribute__((unused))
#define USED __attribute__((used))
#define USED_SECTION(_section) __attribute__((used, section(_section)))

#define EXTERNAL(linker_symbol) extern size_t linker_symbol[];
#define SECTION(_section) __attribute__((section(_section)))

#define likely(x) __builtin_expect(!!(x), 1)
#define unlikely(x) __builtin_expect(!!(x), 0)

#endif  // MOON_DEFS_H
