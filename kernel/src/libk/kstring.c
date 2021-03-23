#include "kstring.h"

// Copy len bytes from src to dest
void memcpy ( uint8_t *dest, const uint8_t *src, uint64_t len )
{
	for ( ; len != 0; len -= 1 )
	{
		*dest = *src;

		dest += 1;
		src  += 1;
	}
}

// Write len copies of val into dest
void memset ( uint8_t *dest, uint8_t val, uint64_t len )  // byte-addressable
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
uint64_t memcmp(const void *src, void *dst, uint64_t n)
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
char* strcpy (char *dest, const char *src)
{
    int n = -1;
    int len = strlen(src);
    do {
        ++n;
        memcpy((uint8_t*)dest, (uint8_t*)src, n);
    } while (n < len);
	
    dest[n] = 0; //Null terminate the stri
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

char *reverse(char *src)
{
    int len = strlen(src);
    volatile char *out; //NO TOUCHY! The compiler will optimize this string for some reason. If the volatile keyword is removed an empty debug or kprintf statement must be inserted before returning `out`. 
    int src_string_index = -1;
    int  last_char = len - 1;

    do {
        ++src_string_index;
        out[src_string_index] = src[last_char];
        last_char--; //Get the last index and move on backwards from there (by "get" I just mean the value of last_char which happens to be the index of the current char)
    } while(src_string_index < len);
    
    out[src_string_index] = 0; // NULL terminate the string
    return (char*)out;
}
