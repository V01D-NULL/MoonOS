// #include <moon-sys/handover.h>
#include <fcntl.h>
#include <mm/phys.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <unity.h>
#include "mock_addr.h"
#include "mock_printk.h"
#include "mock_serial.h"
#include "mock_spinlock.h"

#define MEM_SIZE MiB(5) * sizeof(uint32_t)
#define MAP_ADDR 0x40000

void setUp(void)
{
    impl_verbose_debug_CMockIgnore();
    impl_quiet_debug_CMockIgnore();
    printk_CMockIgnore();
    acquire_lock_CMockIgnore();
    release_lock_CMockIgnore();
}

void test_PhysAllocator(void)
{
    int       fd;
    uint32_t *mapped_mem;

    // Create a file descriptor for anonymous mapping
    fd = open("/dev/zero", O_RDWR);
    if (fd < 0)
    {
        perror("open");
        TEST_FAIL_MESSAGE("Failed to open /dev/zero");
    }

    // Map memory to the desired address
    mapped_mem = mmap((void *)MAP_ADDR,
                      MEM_SIZE,
                      PROT_READ | PROT_WRITE,
                      MAP_PRIVATE | MAP_ANONYMOUS | MAP_FIXED,
                      fd,
                      0);
    if (mapped_mem == MAP_FAILED)
    {
        perror("mmap");
        close(fd);
        TEST_FAIL_MESSAGE("Failed to map memory");
    }

    memset(mapped_mem, 0, MEM_SIZE);

    MemoryMapEntry **memory_entries =
        (MemoryMapEntry **)malloc(sizeof(MemoryMapEntry *) * 5);

    // Give each mmap entry 1 mib
    for (int i = 0; i < 5; i++)
    {
        memory_entries[i] = (MemoryMapEntry *)malloc(sizeof(MemoryMapEntry));
        memory_entries[i]->base   = (uint64_t)mapped_mem + (i * MiB(1));
        memory_entries[i]->length = MiB(1);
        memory_entries[i]->type =
            i % 2 == 0 ? HANDOVER_MEMMAP_USABLE : HANDOVER_MEMMAP_RESERVED;
        printf("Memory entry %d: %p\n", i, memory_entries[i]->base);
    }

    HandoverMemoryMap mmap = {.entry_count = 5, .entries = memory_entries};

    init_phys_allocator(mmap);

    for (int i = 0; i < 10; i++)
    {
        void *ptr = arch_alloc_page_sz(4096);
        TEST_ASSERT_NOT_NULL(ptr);
    }

    if (munmap(mapped_mem, MEM_SIZE) < 0)
        TEST_FAIL_MESSAGE("Failed to unmap memory");

    close(fd);
    free(memory_entries);
}