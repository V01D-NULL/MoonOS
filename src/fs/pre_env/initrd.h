// #ifndef INITRD_H
// #define INITRD_H

// #include "../../drivers/screen/monitor.h"
// #include "../../mem/heap.h"
// #include "../../common.h"
// #include "../vfs.h"

// typedef struct initrd_header {
//     u32int file_count;
// } initrd_header_t;

// typedef struct initrd_file_header {
//     unsigned char magic;
//     char name [128];
//     unsigned int offset;
//     unsigned int length;
// } initrd_file_header_t;

// vfs_node_t *init_initrd(u32int location);

// #endif // INITRD_H