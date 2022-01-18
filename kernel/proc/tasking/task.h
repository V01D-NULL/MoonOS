#ifndef TASK_H
#define TASK_H

#include <mm/vmm.h>
#include <stdint.h>
#include <proc/elf/elf64.h>

enum task_type
{
    /* Special microkernel service/daemon/server */
    TASK_DAEMON = 0,

    /* Regular user process */
    TASK_PROCESS = 1
};

typedef struct task_struct
{
    const char *descriptor;
    uint64_t *pagemap;
    Elf64_Addr entrypoint;
    enum task_type task_type;
    size_t ustack;
} task_t;

task_t new_task(const char *descriptor, Elf64_Addr entrypoint);
void add_task(task_t task);

#endif // TASK_H