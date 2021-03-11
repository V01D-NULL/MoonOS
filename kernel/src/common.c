#include "common.h"
#include "drivers/screen/monitor.h"
#include <bits/stdint-uintn.h>

// Copy len bytes from src to dest
void memcpy ( uint8_t *dest, const uint8_t *src, uint32_t len )
{
	for ( ; len != 0; len -= 1 )
	{
		*dest = *src;

		dest += 1;
		src  += 1;
	}
}

// Write len copies of val into dest
void memset ( uint8_t *dest, uint8_t val, uint32_t len )  // byte-addressable
{
	for ( ; len != 0; len -= 1 )
	{
		*dest = val;

		dest += 1;
	}
}


// Returns 1 if src is less than dst
// Returns -1 if src is greater than dst
// Returns 0 if the length of src and dst are equal
uint64_t memcmp(const void *src, void *dst, int n)
{
    const uint8_t *src_a = src;
    uint8_t *dst_a = dst;
    
    for (int i = 0; i < n; i++)
    {
        if (src_a[i] < dst_a[i])
            return 1;
        else if (src_a[i] > dst_a[i])
            return -1;
    }
    return 0;
}

// Compare two strings.
/* Should return,
     -1 if ( len(str1) > len(str2) ) or ( len(str2) > len(str1) )  // sort of. Shouldn't be used for len comp as will report mismatch first
      0 if str1 == str2
      1 otherwise
*/
int strcmp ( char *str1, char *str2 )
{
	int i = 0;
	int failed = 0;

	while ( str1[ i ] != '\0' && str2[ i ] != '\0' )  // haven't reached null-terminal of either string
	{
		if ( str1[ i ] != str2[ i ] )
		{
			failed = 1;

			break;
		}

		i += 1;
	}

	// did loop exit due to unequal lengths?
	if ( ( str1[ i ] == '\0' && str2[ i ] != '\0' ) ||  // len( str2 ) > len( str1 )
	     ( str1[ i ] != '\0' && str2[ i ] == '\0' ) )   // len( str1 ) > len( str2 )
	{
		failed = - 1;
	}

	return failed;
}

// Copy the NULL-terminated string src into dest, and return dest
char* strcpy ( char dest[100], const char src[100] )
{
	for (int i = 0; src[i] != '\0'; i++)
	{
		dest[i] = src[i];
	}

	return dest;
}

char* strcat(char* a, char* b)
{
    char* ptr = a + strlen(a);
    
    while (*b != '\0')
    	*ptr++ = *b++;

    // null terminate destination string
    *ptr = '\0';

    return a;
}


// Get the string length
uint64_t strlen ( const char *s )
{
	uint64_t len = 0;

	while ( *s++ != '\0' )
	{
		len++;
	}

	return len;	
}


uint64_t decToHex(int n)
{
    // char array to store hexadecimal number 
    char hexaDeciNum[100];

    // counter for hexadecimal number array 
    int i = 0; 

    while(n != 0) 
    {    
        // temporary variable to store remainder 
        int temp  = 0;

        // storing remainder in temp variable. 
        temp = n % 16;

        // check if temp < 10 
        if(temp < 10)
        { 
            hexaDeciNum[i] = temp + 48; 
            i++; 
        } 

        else
        { 
            hexaDeciNum[i] = temp + 55; 
            i++; 
        } 
        n = n/16; 
    } 
    
	char result[100];
    
	for(int j=i-1; j>=0; j--)
        result[j] = hexaDeciNum[j];
	
	//Reverse string. Actually this just puts it back into the correct order.
    char* reversed_string = str_reverse(result);
	
	//Convert the string to a decimal datatype
	uint64_t final = atoi(reversed_string);
	
	//Note: It only returns it as an integer (0x1000 would only be 1000)
    return final;
}

char* str_reverse(char str[])
{
    char temp;
    int i, j = 0;
    i = 0;
    j = strlen(str) - 1; // counting the length
    while (i < j) // for reversing string
    {        
        temp = str[i];
        str[i] = str[j];
        str[j] = temp;
        i++;
        j--;
    }
    return str;
}

uint64_t atoi(const char* string)
{
	int result = 0;
	
	for (int i = 0; i < strlen(string); i++)
	{
		result = result * 10 + string[i] - '0';
	}

	return result;
}

void delay(int time) {
     volatile int i, j;
     for (i=0; i<time; i++)
     {
         for (j=0; j < 250000; j++)
         {
            __asm__ __volatile__("nop");
         }
     }
}

// The boolean is_caller_debug is used sothat a debug() function call with the %b symbol will redirect the output to the serial and not the screen
void to_bin(unsigned int n, bool is_caller_debug)
{
    int a[10], i;
    for(i=0;n>0;i++)    
    {    
        a[i]=n%2;    
        n=n/2;    
    }    
    
    for(i=i-1;i>=0;i--)    
    {
        if (!is_caller_debug)
            kprintf("%d",a[i]);
        else
            debug("%d", a[i]);
	}
}