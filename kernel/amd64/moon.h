#ifndef MOON_DEFS_H
#define MOON_DEFS_H

//Some #define's were taken from https://github.com/GENADEV/GENADEV_OS/blob/main/src/kernel/arm-v-8/genadev.h (Technically I wrote those so it doesn't really matter)
#define __export extern 
#define __no_optimize volatile
#define __no_return __attribute__((noreturn)) void
#define __section_align  __attribute__((aligned (16)))
#define __section_align8 __attribute__((aligned (8)))
#define __section_align4k __attribute__((aligned (4096)))
#define __pack_bytes __attribute__((packed))
#define __asm_flavor_intel ".intel_syntax noprefix\n"
#define __asm_flavor_at_t  ".att_syntax prefix\n"
#define __page_align __attribute__((aligned(4096)))

#define IMPORT_LDDEF(linker_symbol) __export size_t linker_symbol[];
#define __SECTION(_section) __attribute__((section(_section)))
#define __DEPRECATION_INBOUND __attribute__((warning("This functionality is marked for deprecation. Please update your code to meet the latest requirements ASAP.")))
#define __DEPRECATION_OVERDUE __attribute__((error ("This functionality is marked deprecated. Please update your code to meet the latest requirements to ensure a successful compilation of the Moon kernel.")))
#define BINARY_IMPORT(name) __asm__(".global %1\n%1: .incbin %0\n" :: "r"(name) "r"(name));

#endif // MOON_DEFS_H
