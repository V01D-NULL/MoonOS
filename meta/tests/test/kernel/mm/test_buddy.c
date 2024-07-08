#include <stdio.h>
#define BUDDY_ALLOC_IMPLEMENTATION
#define BUDDY_PRINTF printf
#include <mm/buddy.h>
#include <stdint.h>
#include <stdlib.h>
#include <unity.h>

static uint32_t static_memory[16384] = {0};

void test_BuddyEmbedSucceeds(void)
{
    struct buddy *instance = buddy_embed((void *)static_memory, 16384);
    TEST_ASSERT_NOT_NULL(instance);
}

void test_BuddyAllocationDeallocation(void)
{
    struct buddy *instance = buddy_embed((void *)static_memory, 16384);
    size_t        initial_free_memory = buddy_arena_free_size(instance);

    void *ptr = buddy_malloc(instance, 1024);

    TEST_ASSERT_NOT_NULL(ptr);
    TEST_ASSERT_TRUE(buddy_arena_free_size(instance) < initial_free_memory);

    buddy_free(instance, ptr);
    TEST_ASSERT_EQUAL_UINT32(
        buddy_arena_free_size(instance), initial_free_memory);
}