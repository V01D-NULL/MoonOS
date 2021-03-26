#include "linear_alloc.h"

linear_tracker_t mem_manager;
boot_info_t glob_mmap;

void linear_mm_init(void *start, uint64_t size, boot_info_t scoped_mmap)
{
    //Is the size of the allocation buffer valid?
    assert(size > 0);

    //Can we allocate any memory at all or is the RAM full?
    assert(size > scoped_mmap.mmap.free_ram);

    //Init size, start & first free addr
    mem_manager.size = size;
    mem_manager.start = start;
    mem_manager.first_free_addr = start;

    glob_mmap = scoped_mmap;
}

void linear_alloc(uint64_t size/*, bool page_align*/) {
    //1. Find first free address
    //2. buffer size += this.size
    //3. page_align (if true)
    //4. Update RAM usage struct (ram.c/ram.h)
}

// I hesitate to call this linear_free() because linear allocation doesn't really free() memory, it free's the whole buffer
void linear_release_buffer() {

}
