#include <proc/uspace/userspace.h>
#include <proc/elf/elf.h>
#include <panic.h>

void load_daemon(const uint8_t *elf, const char *name)
{
    // Note: identity_map = true basically assumes that the load address of the ELF is free, this might be removed in the future.
    task_t task = load_elf(elf, (struct elf_loader_args){.do_panic = true, .identity_map = true, .descriptor = name});

    if (!task.entrypoint)
        panic("Failed to load ELF; Cannot start kernel daemon!");

    asm("swapgs");
    switch_pagemap(task);
    enter_ring3((void *)task.entrypoint, task.ustack);
}