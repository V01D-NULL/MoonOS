#define PR_MODULE "execution-space"
#include "create.h"
#include <base/align.h>
#include <base/mem.h>
#include <mm/page.h>
#include <mm/phys.h>
#include <mm/virt.h>
#include <printk.h>
#include <service/execution-context/create.h>
#include "loader/elf.h"

EsCreateResult create_execution_space(const uint8_t *elf_pointer)
{
    ExecutionSpace execution_space = {
        .vm_space      = arch_create_new_pagemap(),
        .stack_pointer = (uint64_t)pa(arch_alloc_page_sz(PAGE_SIZE)),
    };

    if (!execution_space.vm_space)
        return Error(EsCreateResult, "Failed to create new pagemap");

    if (!execution_space.stack_pointer)
        return Error(EsCreateResult, "Failed to allocate stack");

    arch_copy_kernel_mappings(execution_space.vm_space);
    arch_map_page(execution_space.vm_space,
                  execution_space.stack_pointer,
                  execution_space.stack_pointer,
                  MAP_USER_RW);

    uint64_t elf_entry_point = TRY_UNWRAP(
        load_elf(elf_pointer, execution_space.vm_space), EsCreateResult);

    execution_space.ec =
        create_ec(elf_entry_point, execution_space.stack_pointer);

    return Okay(EsCreateResult, execution_space);
}
