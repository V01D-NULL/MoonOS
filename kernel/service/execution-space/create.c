#define PR_MODULE "execution-space"
#include "create.h"
#include <mm/alloc.h>
#include <mm/virt.h>
#include <printk.h>
#include <service/execution-context/create.h>
#include "loader/elf.h"

EsCreateResult create_execution_space(const uint8_t *elf_pointer)
{
    ExecutionSpace execution_space = {
        .vm_space      = arch_create_new_pagemap(),
        .stack_pointer = alloc(0x2000),
    };

    if (!execution_space.vm_space)
        return Error(EsCreateResult, "Failed to create new pagemap");

    if (!execution_space.stack_pointer)
        return Error(EsCreateResult, "Failed to allocate stack");

    arch_copy_kernel_mappings(execution_space.vm_space);

    uint64_t elf_entry_point = TRY_UNWRAP(
        load_elf(elf_pointer, execution_space.vm_space), EsCreateResult);

    execution_space.ec =
        create_ec(elf_entry_point, execution_space.stack_pointer);

    trace(TRACE_ELF, "Entry point: %#lx\n", execution_space.ec.entry);

    return Okay(EsCreateResult, execution_space);
}