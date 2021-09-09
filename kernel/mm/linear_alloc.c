#include <stdbool.h>
#include "../util/ptr.h"
#include "linear_alloc.h"
#include "ram.h"

linear_tracker_t mem_manager;

int linear_mm_init(void *start, uint64_t size)
{
    serial_set_color(BASH_GREEN);

    //Is the size of the allocation buffer valid?
    if (size <= 0) {
        serial_set_color(BASH_YELLOW);
        debug(true, "linear_mm_init: %ld is an invalid buffer size\n", size);
        serial_set_color(BASH_WHITE);
        return EXIT_FAILURE;
    }

    //Can we allocate any memory at all or is the RAM full?
    assert(ram_manager_get_free() > size);

    //Init size, start & first free addr
    mem_manager.size = size;
    mem_manager.start = start;
    mem_manager.first_free_addr = start;
    mem_manager.allocation_ptr = start;
    mem_manager.end = VAR_TO_VOID_PTR(size_t, GENERIC_CAST(uintptr_t, start) + size); //End address of the linear memory buffer
    
    debug(true, "linear_mm_init: LinearAllocation pool start address: 0x%x\nlinear_mm_init: LinearAllocation pool end address: 0x%x\n", (size_t)mem_manager.start, (size_t)mem_manager.end);
    serial_set_color(BASH_WHITE);

    return EXIT_SUCCESS;
}

uint8_t *linear_alloc(uint64_t size, int byte_align_ammount) {
    
    serial_set_color(BASH_GREEN);

    //Are we trying to allocate memory without creating a Allocation Pool (possible causes could be not calling linear_mm_init() or allocating memory after freeing the buffer)
    if (mem_manager.start == mem_manager.end) {
        serial_set_color(BASH_YELLOW);
        debug(true, "linear_alloc: You must initialise a linear Allocation pool first!\n");
        serial_set_color(BASH_WHITE);
        return (uint8_t*)EXIT_FAILURE;
    }

    //Are we trying to allocate more memory than we have in the Allocation Pool?
    assert((void*)(size + mem_manager.first_free_addr) <= mem_manager.end);

    //Do we have enough RAM to allocate the memory?
    assert((align((size_t)(mem_manager.first_free_addr += size), byte_align_ammount)) <= ram_manager_get_free()); //Update the first free address
    mem_manager.allocation_ptr = mem_manager.first_free_addr;

    debug(true, "linear_alloc: Allocated %ld bytes, current buffer address: 0x%x (AllocationPool: 0x%x - 0x%x)\n", size, mem_manager.allocation_ptr, mem_manager.start, mem_manager.end);
    
    debug(true, "linear_alloc: ");
    serial_set_color(BASH_WHITE);
    //Update RAM statistics
    uint64_t old_ram = ram_manager_get_used();
    ram_manager_add(size);
    
    debug(true,
        "RAM statistics (used/total):\n"
        "(old) %lld mb/%lld GB ~ %lld/%lld kb\n"
        "(new) %lld mb/%lld GB ~ %lld/%lld kb\n",
            /* old RAM stat */
            old_ram / 1024, ram_manager_get_total() / 1024 / 1024,
            old_ram, ram_manager_get_total(),

            /* new RAM stat */
            ram_manager_get_used() / 1024, ram_manager_get_total() / 1024 / 1024,
            ram_manager_get_used(), ram_manager_get_total()
    );
    
    return mem_manager.allocation_ptr;
}

// I hesitate to call this linear_free() because linear allocation doesn't really free() memory, it free's the whole buffer
void linear_mm_release_buffer() {
    serial_set_color(BASH_GREEN);
    debug(true, "linear_mm_release_buffer: Releasing LinearAllocation Pool which is %ld bytes large\n", (uint64_t)mem_manager.start + (uint64_t)mem_manager.end);
    mem_manager.allocation_ptr = 0;
    mem_manager.first_free_addr = 0;
    mem_manager.size = 0;
    mem_manager.start = 0;
    mem_manager.end = 0;
    serial_set_color(BASH_WHITE);
}
