#ifndef COMMON_UTIL_H
#define COMMON_UTIL_H

#include <stdint.h>

inline void outb(uint16_t port, uint8_t val)
{
    asm volatile ( "outb %0, %1" : : "a"(val), "Nd"(port) );
}

inline void outw(uint16_t port, uint16_t val)
{
    asm volatile ( "outw %0, %1" : : "a"(val), "Nd"(port) );
}

inline void outl(uint16_t port, uint32_t val)
{
    asm volatile ( "outl %0, %1" : : "a"(val), "Nd"(port) );
}

inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    asm volatile ( "inw %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

inline uint32_t inl(uint16_t port)
{
    uint32_t ret;
    asm volatile ( "inb %1, %0"
                   : "=a"(ret)
                   : "Nd"(port) );
    return ret;
}

inline void io_wait(void) {
    inb(0x80);
}

inline void delay(int time) {
     volatile int i, j = 0;
     for (i=0; i<time; i++)
     {
         for (j=0; j < 250000; j++)
         {
            asm volatile("nop");
         }
     }
}

inline uint64_t align(int size, int align)
{
    return (size + align - 1) & ~(align - 1);
}

#endif  // COMMON_H
