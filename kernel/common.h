/**
 * @file common.h
 * @author Tim (V01D)
 * @brief Common functions, typedefs, etc
 * @version 0.1
 * @date 2021-04-15
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

//Checks if the bit is set. Returns 1 if true, 0 if false.
// Input is the variable containing the value to be checked, bit_index = Nth bit to check.
#define CHECK_BIT(variable, bit_index)      (((variable)>>(bit_index)) & 1) //First bit shift to the given bit index (N). Then simply OR it with 1 to check if it is set.
#define SET_BIT(variable, bit_index, value) (((variable)>>bit_index) | value)

//Get the lower or upper 32 bits of a 64 bit data type
#define LOWER_32(variable) (variable & 0xffffffff)
#define UPPER_32(variable) (variable << 32)
#define DATATYPE_SZ(variable) ( sizeof(variable) * 8 )

#define BASE_2  2
#define BASE_8  8
#define BASE_10 10
#define BASE_16 16

#define true 1
#define false 0
#define null (void*)0
typedef int bool;

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

inline void io_wait() {
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
