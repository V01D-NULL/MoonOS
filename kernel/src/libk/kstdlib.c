#include "kstdlib.h"
#include "kstring.h"
#include "../common.h"

uint64_t atoi(const char* string)
{
	int result = 0;
	
	for (int i = 0; i < strlen(string); i++)
	{
		result = result * 10 + string[i] - '0';
	}

	return result;
}



char *itob(uint64_t n) {
    int sign;
    char result[10];
    if ((sign=n)<0)
        n = -n;

    do {
        int i = n % BASE_2;
        
    }while((n /= 2) > 0);

    return "itob() IS NOT YET READY\n";
}

char *itoh(uint64_t n) {
    return "itoh() IS NOT YET READY\n";
}

char *itoa(uint64_t n, char *str) {
    static int i = 0;
    if (n == 0) {
        str[i] = '0';
        str[++i] = '\0';
    }

    while (n != 0)
    {
        str[i++] = '0' + n;
    }

    return str;
    // return "itoa() IS NOT YET READY\n";
}