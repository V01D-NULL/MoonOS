#include "base/string.h"

// Compare two strings.
/* Returns:
     -1 if ( len(str1) > len(str2) ) or ( len(str2) > len(str1) )
      0 if str1 == str2
      1 otherwise
*/
int strcmp (string_view str1, string_view str2)
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

int strncmp(string_view str1, string_view str2, size_t n)
{
	for (size_t i = 0; i < n && str1; i++)
	{
		if (str1[i] != str2[i])
			return -1;
	}
	return 0;
}

// Copy the NULL-terminated string src into dest, and return dest
char* strcpy (char *dest, string_view src)
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
uint64_t strlen (string_view s)
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

bool isdigit(int c)
{
    return (c >= '0' && c <= '9');
}
