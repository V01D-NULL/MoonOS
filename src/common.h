// Defines typedefs and some global functions

#ifndef COMMON_H
#define COMMON_H

//Checks if the bit is set. Returns 1 if true, 0 if false.
// Input is the variable containing the value to be checked, bit_index = Nth bit to check.
#define CHECK_BIT(variable, bit_index) (((variable)>>(bit_index)) & 1) //First bit shift to the given bit index (N). Then simply OR it with 1 to check if it is set.

//
#define NBITS 32

#define true 1
#define false 0
#define null (void*)0

// Some nice typedefs to standardize sizes across platforms
// These typedefs are written for 32-bit x86
typedef unsigned int   u32int;
typedef          int   s32int;
typedef unsigned short u16int;
typedef          short s16int;
typedef unsigned char  u8int;
typedef          char  s8int;
typedef          int   bool;

//io.s
extern void outb(u16int port,  u8int val);
extern void outw(u16int port,  u16int val);
extern void outl(u16int port,  u32int val);
extern u8int  inb(u16int port);
extern u16int inw(u16int port);
extern u32int inl(u16int port);

// Read a byte from the specified port
// u8int inb ( u16int port );

// Read a short from the specified port
// u16int inw ( u16int port );

// Copy len bytes from src to dest
void memcpy ( u8int *dest, const u8int *src, u32int len );

// Write len copies of val into dest?
void memset ( u8int *dest, u8int val, u32int len );

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
