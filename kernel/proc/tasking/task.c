#include "task.h"
#include <printk.h>
#include <mm/mm.h>
static task_t tasks[10];
static int counter = 0;

void add_task(task_t task)
{
    printk("task", "Task: %s | Pagemap: 0x%lX | Virtual entrypoint: 0x%lx\n", task.descriptor, task.pagemap, task.entrypoint);
    tasks[counter++] = task;
}

task_t new_task(const char *descriptor, Elf64_Addr entrypoint)
{
    return (task_t){.descriptor = descriptor, .pagemap = create_new_pagemap(), .entrypoint = entrypoint, .ustack = 0}; // The ustack resides next to the ELF mappings (example: ELF: 0x400000-0x401000 | ustack: 0x401000-0x403000)
}