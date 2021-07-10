#include "CR.h"

static inline uint64_t cr0_read()
{
    uint64_t cr;
    asm volatile(
        "mov %%cr0, %0\n\t"
        : "=r"(cr)
        :
        : "%rax"
    );
    return cr;
}

static inline uint64_t cr1_read()
{
    uint64_t cr;
    asm volatile(
        "mov %%cr1, %0\n\t"
        : "=r"(cr)
        :
        : "%rax"
    );
    return cr;
}

static inline uint64_t cr2_read()
{
    uint64_t cr;
    asm volatile(
        "mov %%cr2, %0\n\t"
        : "=r"(cr)
        :
        : "%rax"
    );
    return cr;
}

static inline uint64_t cr3_read()
{
    uint64_t cr;
    asm volatile(
        "mov %%cr3, %0\n\t"
        : "=r"(cr)
        :
        : "%rax"
    );
    return cr;
}

static inline uint64_t cr4_read()
{
    uint64_t cr;
    asm volatile(
        "mov %%cr4, %0\n\t"
        : "=r"(cr)
        :
        : "%rax"
    );
    return cr;
}

uint64_t cr_read(enum CR_NUMBER cr)
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