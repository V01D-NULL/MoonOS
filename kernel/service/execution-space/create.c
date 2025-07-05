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
                                      ArgumentVector argv)
{
    ExecutionSpace execution_space = {
        .pid = sched_new_pid(),  // TODO: This is a quick hack to get things
                                 // working. This requires a refactor to avoid
                                 // race conditions, duplicate pids, etc.
        .port          = -1,
        .message_queue = NULL,
        .vm_space      = arch_create_new_pagemap(),
        .stack_pointer = (uint64_t)pa(arch_alloc_page_sz(PAGE_SIZE)) +
                         PAGE_SIZE,  // + PAGE_SIZE to get the top of the stack
    };

    if (!execution_space.vm_space)
        return Error(EsCreateResult, "Failed to create new pagemap");

    if (!execution_space.stack_pointer)
        return Error(EsCreateResult, "Failed to allocate stack");

    arch_copy_kernel_mappings(execution_space.vm_space);
    arch_map_page(execution_space.vm_space,
                  execution_space.stack_pointer - PAGE_SIZE,
                  execution_space.stack_pointer - PAGE_SIZE,
                  MAP_USER_RW);

    int argc = execution_space.argc = argv != NULL ? size(&argv) : 0;

    if (argc > 0)
    {
        auto argv_base = alloc_aligned((argc + 1) * sizeof(string), 16);

        execution_space.argv = pa(argv_base);
        arch_map_page(execution_space.vm_space,
                      pa(argv_base),
                      pa(argv_base),
                      MAP_USER_RO);

        for (int i = 0; i < size(&argv); i++)
        {
            auto   arg        = *get(&argv, i);
            size_t arg_length = strlen(arg) + 1;  // +1 for null terminator

            char *arg_copy = (char *)alloc_aligned(arg_length, 16);
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
