#ifndef MOON_DEFS_H
#define MOON_DEFS_H

#include <stddef.h>

// gnu_* refers to the gnu specific features such as alignment or plain C keywords
// #define extern extern 
// #define volatile volatile
#define gnu_no_return __attribute__((noreturn))
#define gnu_section_align8 __attribute__((aligned (8)))
#define gnu_section_align16  __attribute__((aligned (16)))
#define gnu_section_align4k __attribute__((aligned (4096)))
#define gnu_pack_bytes __attribute__((packed))
#define gnu_unused __attribute__((unused))
#define gnu_asm_flavor_intel ".intel_syntax noprefix\n"
#define gnu_asm_flavor_at_t  ".att_syntax prefix\n"

#define STATIC_INLINE __attribute__((always_inline)) static inline
#define EXTERNAL(linker_symbol) extern size_t linker_symbol[];
#define SECTION(_section) __attribute__((section(_section)))
#define DEPRECATION_INBOUND __attribute__((warning("This functionality is marked for deprecation. Please update your code to meet the latest requirements ASAP.")))
#define DEPRECATION_OVERDUE __attribute__((error ("This functionality is marked deprecated. Please update your code to meet the latest requirements to ensure a successful compilation of the Moon kernel.")))
#define MAYBE_UNUSED __attribute__((maybe_unused))
#define USED __attribute__((used))

#endif // MOON_DEFS_H
