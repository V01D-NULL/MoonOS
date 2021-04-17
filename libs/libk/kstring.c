#include "kstring.h"

// Copy len bytes from src to dest
void memcpy (uint8_t *dest, const uint8_t *src, uint64_t len)
{
	for (; len != 0; len--)
	{
		*dest = *src;

		dest++;
		src++;
	}
}

void memset(uint8_t *dest, uint8_t val, uint64_t len)
{
	for (; len != 0; len--)
	{
		*dest = val;
		dest++;
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
     -1 if ( len(str1) > len(str2) ) or ( len(str2) > len(str1) )
      0 if str1 == str2
      1 otherwise
*/
int strcmp (char *str1, char *str2)
{
	int i = 0;
	int failed = 0;

	while (str1[i] != '\0' && str2[i] != '\0')
	{
		if (str1[i] != str2[i])
		{
			failed = 1;
		
			break;
		}

		i++;
	}

	// did loop exit due to unequal lengths?
	if ((str1[i] == '\0' && str2[i] != '\0') ||  // len( str2 ) > len( str1 )
	    (str1[i] != '\0' && str2[i] == '\0' ))   // len( str1 ) > len( str2 )
	{
		failed = -1;
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
uint64_t strlen (const char *s)
{
	uint64_t len = 0;

	while (*s++ != '\0')
	{
		len++;
	}

	return len;	
}

char *strrev(char *src)
{
	static char temp;
	int src_string_index = 0;
	int last_char = strlen(src) - 1;

	for (; src_string_index < last_char; src_string_index++)
	{
		temp = src[src_string_index]; 			  // Save current character
		src[src_string_index] = src[last_char];   // Swap out the current char with the last char
		src[last_char] = temp;	 				  // Swap out last character with the current character
		last_char--;
	}

	src[strlen(src)-1+1] = 0;

    return src;
}
