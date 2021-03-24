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

//itob and itoh will be left empty for now. All they do is do what itoa does but without specifying the base since the function knows it already
char *itob(uint64_t n) {

    return "itob() -> not yet implemented. Please use itoa() instead\n";
}

char *itoh(uint64_t n) {
    return "itoh() -> not yet implemented. Please use itoa() instead\n";
}

char *itoa(int n, char *str, int base) {
    int i = 0;
    bool is_signed = false;

    if (n == 0) {
        str[i] = '0';
        str[i++] = '\0';
        return str;
    }

    is_signed = true ? (base == BASE_10 && n < 0) : false;
    if (is_signed) n = -n;

    while (n != 0)
    {
        int remainder = n % base;
        str[i++] = (remainder > 9) ? (remainder-10) + 'a' : remainder + '0';
        n /= base;
    }
    
    if (is_signed)
        str[i++] = '-';

    str[i] = '\0'; //NULL terminate string
    
    return strrev(str);
}