#include "pmm.h"

extern u32int end;
u32int paddr = (u32int)&end;

void print_end()
{
    kprintf("%x\n", paddr);
}