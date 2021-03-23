// Defines typedefs and some global functions

#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

//Checks if the bit is set. Returns 1 if true, 0 if false.
// Input is the variable containing the value to be checked, bit_index = Nth bit to check.
#define CHECK_BIT(variable, bit_index)      (((variable)>>(bit_index)) & 1) //First bit shift to the given bit index (N). Then simply OR it with 1 to check if it is set.
#define SET_BIT(variable, bit_index, value) internal_set_bit(variable, bit_index, value)

//Get the lower or upper 32 bits of a 64 bit data type
#define LOWER_32(variable) (variable & 0xffffffff)
#define UPPER_32(variable) (variable >> 32)

//
#define NBITS 32

#define true 1
#define false 0
#define null (void*)0
typedef int bool;

//Sets a bit at a given index and returns it.
//This is just a wrapper function for the SET_BIT macro 
static inline uint64_t internal_set_bit(uint64_t variable, uint64_t bit_index, uint64_t value)
{
    return (((variable)>>bit_index) | value);
}

//io.s
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

// Copy len bytes from src to dest
void memcpy ( uint8_t *dest, const uint8_t *src, uint32_t len );

// Set dest equal to val until len is met
void memset ( uint8_t *dest, uint8_t val, uint32_t len );

//Compare two pointers/bytes with a length of n
uint64_t memcmp(const void *src, void *dst, int n);

// Compare two strings
int strcmp ( char *str1, char *str2 );

// Copy the NULL-terminated string src into dest, and return dest
char* strcpy ( char dest[100], const char src[100] );

// Concatenate the NULL-terminated string src onto the end of dest, and return dest
char* strcat(char* a, char* b);

// Get the string length
uint64_t strlen ( const char *s );

//Convert a decimal string to hexadecimal
uint64_t decToHex(int n);

// Reverse a string
char* str_reverse(char str[]);

//Ascii to int
uint64_t atoi(const char* string);

//
void delay(int time);

//
char* to_dec(uint64_t n);

//
void to_bin(unsigned int n, bool is_caller_debug);

#endif  // COMMON_H
