#ifndef HEAP_H
#define HEAP_H

void  alloc_init(void);
void *alloc(int sz);
void *alloc_aligned(int sz, int align);
void *realloc(void *ptr, int sz);
void *calloc(int sz);
void  free(void *ptr, int sz);

#endif  // HEAP_H