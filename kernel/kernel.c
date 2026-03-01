#define PR_MODULE "main"

#include <base/fmt.h>
#include <cpu.h>
#include <ipc/ipc.h>
#include <ipc/port.h>
#include <mm/phys.h>
#include <mm/virt.h>
#include <moon-sys/handover.h>
#include <moon-sys/time/sleep.h>
#include <platform.h>
#include <sched/scheduler.h>
#include <service/execution-space/create.h>
#include <sys/syscall.h>

#include "panic.h"
#include "printk.h"

#include <service/capability.h>

vec(Capability) setup_init_task_capabilities(HandoverMemoryMap memory_map);

NORETURN void kern_main(HandoverModules mods, HandoverMemoryMap memory_map)
{
    panic_if(mods.count != 2,
             "Expected 2 handover modules, received '%d'",
             mods.count);

    auto initModule = mods.modules[0];
    panic_if(strncmp(initModule.cmdline, "init.elf", 8) != 0,
             "First handover module is not init process");

    auto ramdiskModule = mods.modules[1];
    panic_if(strncmp(ramdiskModule.cmdline, "ramdisk", 7) != 0,
             "Second handover module is not ramdisk");

    arch_init_syscall();
    sched_prepare();
    ipc_init();

#if !defined(__x86_64__)
    panic("Platform not fully supported");
#endif

    ArgumentVector argv;
    init(&argv);
    push(&argv, "init.elf");

    char buffer[20];
    snprintf(buffer, sizeof(buffer), "0x%p", pa(ramdiskModule.address));
    push(&argv, buffer);

    auto caps = setup_init_task_capabilities(memory_map);

    auto space = UNWRAP(create_execution_space(initModule.address, argv, caps));
    cleanup(&argv);
    cleanup(&caps);

    panic_if(ipc_assign_port(&space, PORT_INIT) == false,
             "Unable to assign IPC port to init process");
    sched_enqueue(space);

    Range range = {.base  = pa(ramdiskModule.address),
                   .limit = pa(ramdiskModule.address) + ramdiskModule.size};
    arch_map_range(space.vm_space, range, MAP_USER_RO, 0);

    sched_begin_work();
    arch_halt_cpu();
}

vec(Capability) setup_init_task_capabilities(HandoverMemoryMap memory_map)
{
    vec(Capability) caps;
    init(&caps);

    bool has_allocatable_region = false;

    // Grant full memory access to init task
    for (int i = 0; i < memory_map.entry_count; i++)
    {
        if ((memory_map.entries[i]->type != HANDOVER_MEMMAP_USABLE &&
             memory_map.entries[i]->type != HANDOVER_MEMMAP_FRAMEBUFFER) ||
            memory_map.entries[i]->length == PAGE_SIZE)
            continue;

        CapabilityMemoryRegionType region_type =
            memory_map.entries[i]->type == HANDOVER_MEMMAP_USABLE
                ? CAP_MEMORY_REGION_RAM
                : CAP_MEMORY_REGION_FRAMEBUFFER;

        Capability region_cap =
            capability_create_memory_region(memory_map.entries[i]->base,
                                            memory_map.entries[i]->length,
                                            true,
                                            region_type);

        // Create 16 MiB allocatable region capability for init task
        if (memory_map.entries[i]->length >= MiB(16) &&
            has_allocatable_region == false)
        {
            has_allocatable_region = true;
            arch_reserve_range((void *)memory_map.entries[i]->base, MiB(16));

            Capability allocatable_region_cap =
                capability_make_region_allocatable(region_cap);

            // Q&D way to steal some memory for the allocatable region
            // capability. Future work: implement a proper way to carve out
            // allocatable regions
            region_cap.data.memory_region.range.base += MiB(16);
            region_cap.data.memory_region.range.limit -= MiB(16);

            if (allocatable_region_cap.type == CAP_BAD)
                panic("Failed to create allocatable region capability");

            push(&caps, allocatable_region_cap);
        }

        push(&caps, region_cap);
    }

    return caps;
}