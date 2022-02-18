#ifndef MM_BUDDY_H
#define MM_BUDDY_H

#include <boot/bootloader_stivale2.h>
#include <mm/mm.h>

void buddy_init(struct stivale2_mmap_entry *mmap, int entries);
void buddy_free(struct page *page);
struct page *buddy_alloc(int order);

#endif // MM_BUDDY_H