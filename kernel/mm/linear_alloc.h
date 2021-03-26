#ifndef MM_LINEAR_H
#define MM_LINEAR_H

#include <stdint.h>
#include <libk/kassert.h>
#include <libk/kstdlib.h>
#include "../drivers/io/serial.h"
#include "../boot/bootloader_stivale2.h"

/*
    Q&D linear memory allocator | Will be replaced by a more advanced memory managment algorithm eventually
*/

typedef struct linear_tracker {
    void *start;
    uint64_t size;
    uint8_t *first_free_addr;
    void *end;
    uint8_t *allocation_ptr;
}linear_tracker_t;

//Setup a linear memory buffer. Returns EXIT_FAILURE (1) on failure, and EXIT_SUCCESS (0) on success
int linear_mm_init(void *start, uint64_t size);

//Allocate a block of the linear buffer's memory. Returns EXIT_FAILURE (1) on failure, and a pointer to the allocated block on success
uint8_t *linear_alloc(uint64_t size/*, int byte_align_ammount*/);

//Release all memory in the linear buffer.
void linear_mm_release_buffer();

#endif // MM_LINEAR_H