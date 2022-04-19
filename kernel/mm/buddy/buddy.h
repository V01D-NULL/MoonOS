#ifndef MM_BUDDY_H
#define MM_BUDDY_H

#include <boot/boot.h>
#include <mm/mm.h>
#define buddy_type_t int // Similar to gfp_t on linux

struct zone *buddy_init(struct buddy_range range, buddy_type_t type);
void buddy_free(struct page page);
struct page buddy_alloc(int order, struct zone *memory_zones);

#endif // MM_BUDDY_H