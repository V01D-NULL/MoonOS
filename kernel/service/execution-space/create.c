#define PR_MODULE "execution-space"
#include "create.h"
#include <base/align.h>
#include <base/mem.h>
#include <ipc/ipc.h>
#include <mm/page.h>
#include <mm/phys.h>
#include <mm/virt.h>
#include <printk.h>
#include <sched/scheduler.h>
#include <service/execution-context/create.h>
#include "loader/elf.h"

EsCreateResult create_execution_space(const uint8_t *elf_pointer,
                                      ArgumentVector argv,
                                      vec(Capability) capabilities)
{
    Capability memory_region_cap = {.type = CAP_BAD};
    for_each(&capabilities, cap)
    {
        if (cap->type & CAP_ALLOCATABLE_MEMORY_REGION)
        {
            memory_region_cap = *cap;
            break;
        }
    }

    if (memory_region_cap.type == CAP_BAD)
        return Error(EsCreateResult, "No memory region capability provided");

    ExecutionSpace execution_space = {
        .pid           = sched_allocate_pid(),
        .port          = -1,
        .message_queue = NULL,
        .capabilities  = NULL,
        .vm_space      = arch_create_new_pagemap(),
        .stack_pointer =
            pa(capability_alloc_from(memory_region_cap, PAGE_SIZE)) + PAGE_SIZE,
    };

    if (!execution_space.vm_space)
        return Error(EsCreateResult, "Failed to create new pagemap");

    if (!execution_space.stack_pointer)
        return Error(EsCreateResult, "Failed to allocate stack");

    init(&execution_space.capabilities);
    for_each(&capabilities, cap) push(&execution_space.capabilities, *cap);

    arch_copy_kernel_mappings(execution_space.vm_space);
    arch_map_page(execution_space.vm_space,
                  execution_space.stack_pointer - PAGE_SIZE,
                  execution_space.stack_pointer - PAGE_SIZE,
                  MAP_USER_RW);

    int argc = execution_space.argc = argv != NULL ? size(&argv) : 0;

    if (argc > 0)
    {
        auto argv_base = capability_alloc_from(
            memory_region_cap, (argc + 1) * sizeof(char *));

        if (!argv_base)
            return Error(EsCreateResult, "Failed to allocate memory for argv");

        execution_space.argv = pa(argv_base);
        arch_map_page(execution_space.vm_space,
                      pa(argv_base),
                      pa(argv_base),
                      MAP_USER_RO);

        for (int i = 0; i < size(&argv); i++)
        {
            auto   arg        = *get(&argv, i);
            size_t arg_length = strlen(arg) + 1;  // +1 for null terminator

            char *arg_copy =
                (char *)capability_alloc_from(memory_region_cap, arg_length);

            strcpy(arg_copy, arg);

            ((char **)argv_base)[i] = pa(arg_copy);
        }

        ((char **)argv_base)[argc] = NULL;
    }

    uint64_t elf_entry_point = TRY_UNWRAP(
        load_elf(elf_pointer, execution_space.vm_space), EsCreateResult);

    execution_space.ec =
        create_ec(elf_entry_point, execution_space.stack_pointer);

    return Okay(EsCreateResult, execution_space);
}
