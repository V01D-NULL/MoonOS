#include <proc/uspace/userspace.h>
#include <proc/elf/elf.h>
#include <printk.h>
#include <panic.h>

void load_daemon(const uint8_t *elf, const char *name)
{
    printk("daemon", "Loading %s...\n", name);
    
    // Note: identity_map = true basically assumes that the load address of the ELF is free, this might be removed in the future.
    task_t task = load_elf(elf, (struct elf_loader_args){.do_panic = true, .identity_map = true, .descriptor = name});

    if (!task.entrypoint)
        panic("Failed to load ELF; Cannot start kernel daemon!");

    printk("daemon", "Successfully loaded %s daemon\n", name);

    asm("swapgs" ::: "memory");
    switch_pagemap(task);
    enter_ring3((void *)task.entrypoint, task.ustack);
}