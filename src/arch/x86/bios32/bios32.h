#ifndef BIOS_32_H
#define BIOS_32_H


/*
    https://web.archive.org/web/20150706011122/http://www-pc.uni-regensburg.de/hardware/TECHDOK/BIOS_32BIT_SERVICE.PDF
*/

typedef struct __attribute__((packed)) {
    unsigned short di, si, bp, sp, bx, dx, cx, ax;
    unsigned short gs, fs, es, ds, eflags;
} regs16_t;

extern void int32(int interrupt, regs16_t *registers);

#endif