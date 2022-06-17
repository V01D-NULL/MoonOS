#define PR_MODULE "smp"

#include "smp.h"
#include <printk.h>
#include <hal/apic/apic.h>
#include <amd64/cpu.h>

typedef uint8_t stack[4096];
static stack stacks[64] = {0};
static volatile int cpu_up = 0;

static void ap_startup(struct stivale2_smp_info *cpu)
{
    pr_info("Started CPU#%d\n", lapic_read(0x20) >> 24);
    init_percpu(cpu->target_stack);
    cpu_up = 1;
    
    for(;;)
    {
        asm("hlt");
    }
}

void smp_init(BootContextSmp *smp)
{
    if (smp->processor_count == 1)
    {
        pr_info("Failed to init smp; there is only 1 cpu\n");
        return;
    }
    
    pr_info("Total CPUs: %d\n", smp->processor_count);
    pr_info("Starting %d other %s\n", smp->processor_count - 1, smp->processor_count > 2 ? "cpus" : "cpu");
    
    for (int i = 0; i < smp->processor_count; i++)
    {
        struct stivale2_smp_info *current = &smp->smp_info[i];
        if (current->lapic_id != smp->bootstrap_processor_lapic_id)
        {
            current->target_stack = (uintptr_t)stacks[current->lapic_id] + sizeof(stack);
            current->goto_address = (uintptr_t)ap_startup;
            while(cpu_up == 0);
            cpu_up = 0;
        }
    }
    pr_info("Initialized all cores\n");
}