#include "CR.h"

static inline uint32_t cr0_read()
{
    uint64_t cr;
    asm volatile(
        "mov %%cr0, %%rax\n\t"
        "mov %%eax, %0\n\t"
        : "=m"(cr)
        :
        : "%rax"
    );
    return cr;
}

static inline uint32_t cr1_read()
{
    uint64_t cr;
    asm volatile(
        "mov %%cr1, %%rax\n\t"
        "mov %%eax, %0\n\t"
        : "=m"(cr)
        :
        : "%rax"
    );
    return cr;
}

static inline uint32_t cr2_read()
{
    uint64_t cr;
    asm volatile(
        "mov %%cr2, %%rax\n\t"
        "mov %%eax, %0\n\t"
        : "=m"(cr)
        :
        : "%rax"
    );
    return cr;
}

static inline uint32_t cr3_read()
{
    uint64_t cr;
    asm volatile(
        "mov %%cr3, %%rax\n\t"
        "mov %%eax, %0\n\t"
        : "=m"(cr)
        :
        : "%rax"
    );
    return cr;
}

static inline uint32_t cr4_read()
{
    uint64_t cr;
    asm volatile(
        "mov %%cr4, %%rax\n\t"
        "mov %%eax, %0\n\t"
        : "=m"(cr)
        :
        : "%rax"
    );
    return cr;
}

uint32_t cr_read(enum CR_NUMBER cr)
{
    switch (cr)
    {
        case 0:
            return cr0_read();

        case 1:
            return cr1_read();

        case 2:
            return cr2_read();

        case 3:
            return cr3_read();
        
        case 4:
            return cr4_read();

        default:
            return 0xff; //-1
    }

}