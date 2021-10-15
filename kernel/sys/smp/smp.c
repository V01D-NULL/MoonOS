#include "smp.h"
#include <printk.h>

typedef uint8_t stack[4096];
static stack stacks[64] = {0};
static volatile int cpu_up = 0;

static void ap_startup(struct stivale2_smp_info *cpu)
{
    printk("smp", "Started CPU#%d\n", cpu->lapic_id);
    cpu_up = 1;
    
    for(;;)
    {
        __asm__("hlt");
    }
}

void smp_init(boot_cpu_t *smp)
{
    printk("smp-init", "Total CPU's: %d\n", smp->processor_count);
    printk("smp-init", "Starting %d other cpu's\n", smp->processor_count - 1);

    if (smp->processor_count == 1)
    {
        printk("smp-init", "Failed to init smp; there is only 1 cpu\n");
        return;
    }
    
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
    printk("smp-init", "Initialized all cores\n");
}