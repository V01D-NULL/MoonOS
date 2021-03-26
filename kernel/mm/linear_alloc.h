#ifndef MM_LINEAR_H
#define MM_LINEAR_H

#include <stdint.h>
#include <libk/kassert.h>
#include "../drivers/io/serial.h"
#include "../boot/bootloader_stivale2.h"

/*
    Q&D linear memory allocator | Will be replaced by a more advanced memory managment algorithm eventually
*/

typedef struct linear_tracker {
    void *start;
    uint64_t size;
    uint8_t *first_free_addr;
    // uint64_t buffer[512]; //[#####*****####**###] (# = used, * = free) (linear_find_free_block() will find a memory in the buffer that isn't occupied and this is turning into a bitmap wtf no stop. I need to sleep. kthxbye)
}linear_tracker_t;

void linear_mm_init(void *start, uint64_t size, boot_info_t scoped_mmap);
void linear_alloc(uint64_t size/*, bool page_align*/);
void linear_release_buffer();

#endif // MM_LINEAR_H