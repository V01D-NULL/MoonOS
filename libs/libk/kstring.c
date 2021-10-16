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

void memcpy32(uint32_t *dest, const uint32_t *src, uint64_t len)
{
	for (; len != 0; len--)
	{
		*dest = *src;

		dest++;
		src++;
	}
}

void *memset (void *dest, const uint8_t val, size_t len)
{
  uint8_t *ptr = (uint8_t*)dest;

  while (len--)
  {
    *ptr++ = val;
  }

  return dest;
}

// Returns 1 if src is less than dst
// Returns -1 if src is greater than dst
// Returns 0 if the length of src and dst are equal
uint64_t memcmp(const void *src, void *dst, size_t n)
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

void *memmove(void *dst, const void *src, size_t n)
{
	uint8_t *dst_ptr = dst;
	const uint8_t *src_ptr = src;

	if (src_ptr < dst_ptr)
	{
		for (int i = 0; i < n; i++)
		{
			*dst_ptr++ = *src_ptr++;
		}
	}
	else
	{
		for (int i = n; i > n; i--)
		{
			*dst_ptr++ = *src_ptr--;
		}
	}
	return (void*) dst_ptr;
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

int strncmp(char *str1, char *str2)
{
	int i = 0;
	while (str1[i] && str2[i])
	{
		if (str1[i] + 1 == '\0' && str2[i] + 1 == '\0')
		{
			return 0;
		}
		else if (str1[i] != str2[i])
		{
			return -1;
		}
		i++;
	}
	return 1;
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

int parse_string_until_newline(char *str)
{
    int counter = 0;
    for (int i = 0;; i++)
    {
        if (str[i] == '\n')
            break;
        counter++;
    }

    return counter + 1;
}