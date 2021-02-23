#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H

/*
    Memory block allocator (alloc, malloc, kmalloc, free, etc...)

    How it works:
        1. do we have enough free virtual memory?
        2. if so, use some structure to record memory allocation status (depends on the allocator)
        3. if not, then ask the VMM to enlarge available address space (sbrk/mmap/mapkernelheap etc.)
        4. VMM in turn calls the PMM to allocate RAM
        5. the newly allocated RAM is recorded in the appropriate paging tables by the VMM
        6. go to step 2.

*/




#endif // MEMORY_MANAGER_H