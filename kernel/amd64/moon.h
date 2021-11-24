#ifndef MOON_DEFS_H
#define MOON_DEFS_H

// gnu_* refers to the gnu specific features such as alignature or plain C keywords
#define gnu_export extern 
#define gnu_no_optimize volatile
#define gnu_no_return __attribute__((noreturn)) void
#define gnu_section_align8 __attribute__((aligned (8)))
#define gnu_section_align16  __attribute__((aligned (16)))
#define gnu_section_align4k __attribute__((aligned (4096)))
#define gnu_pack_bytes __attribute__((packed))
#define gnu_unused __attribute__((unused))
#define gnu_asm_flavor_intel ".intel_syntax noprefix\n"
#define gnu_asm_flavor_at_t  ".att_syntax prefix\n"

#define STATIC_INLINE __attribute__((always_inline)) static inline
#define EXTERNAL(linker_symbol) __export size_t linker_symbol[];
#define SECTION(_section) __attribute__((section(_section)))
#define DEPRECATION_INBOUND __attribute__((warning("This functionality is marked for deprecation. Please update your code to meet the latest requirements ASAP.")))
#define DEPRECATION_OVERDUE __attribute__((error ("This functionality is marked deprecated. Please update your code to meet the latest requirements to ensure a successful compilation of the Moon kernel.")))

#endif // MOON_DEFS_H
