// Defines typedefs and some global functions

#ifndef COMMON_H
#define COMMON_H

//Checks if the bit is set. Returns 1 if true, 0 if false.
// Input is the variable containing the value to be checked, bit_index = Nth bit to check.
#define CHECK_BIT(variable, bit_index)      (((variable)>>(bit_index)) & 1) //First bit shift to the given bit index (N). Then simply OR it with 1 to check if it is set.
#define SET_BIT(variable, bit_index, value) internal_set_bit(variable, bit_index, value)

//
#define NBITS 32

#define true 1
#define false 0
#define null (void*)0

// Some nice typedefs to standardize sizes across platforms
// These typedefs are written for 32-bit x86
typedef unsigned long long u64int; //yes, this does work on 32 bit machines. The compiler will just use two 32 bit registers (same happens to 64 bit machines btw)
typedef signed   long long s64int;
typedef unsigned int   u32int;
typedef          int   s32int;
typedef unsigned short u16int;
typedef          short s16int;
typedef unsigned char  u8int;
typedef          char  s8int;
typedef          int   bool;

//Sets a bit at a given index and returns it.
//This is just a wrapper function for the SET_BIT macro 
static inline u32int internal_set_bit(u32int variable, u32int bit_index, u32int value)
{
    return (((variable)>>bit_index) | value);
}

//io.s
extern void outb(u16int port,  u8int val);
extern void outw(u16int port,  u16int val);
extern void outl(u16int port,  u32int val);
extern u8int  inb(u16int port);
extern u16int inw(u16int port);
extern u32int inl(u16int port);

// Copy len bytes from src to dest
void memcpy ( u8int *dest, const u8int *src, u32int len );

// Set dest equal to val until len is met
void memset ( u8int *dest, u8int val, u32int len );

//Compare two pointers/bytes with a length of n
u32int memcmp(const void *src, void *dst, int n);

// Compare two strings
int strcmp ( char *str1, char *str2 );

// Copy the NULL-terminated string src into dest, and return dest
char* strcpy ( char dest[100], const char src[100] );

// Concatenate the NULL-terminated string src onto the end of dest, and return dest
char* strcat(char* a, char* b);

// Get the string length
u32int strlen ( const char *s );

//Convert a decimal string to hexadecimal
u32int decToHex(int n);

// Reverse a string
char* str_reverse(char str[]);

//Ascii to int
u32int atoi(const char* string);

//
void delay(int time);

//
char* to_dec(u32int n);

//
void to_bin(unsigned int n, bool is_caller_debug);

#endif  // COMMON_H
