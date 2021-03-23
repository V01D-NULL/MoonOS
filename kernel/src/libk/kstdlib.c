#include "kstdlib.h"
#include "kstring.h"

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
    return "itob() IS NOT YET READY\n";
}

char *itoh(uint64_t n) {
    return "itoh() IS NOT YET READY\n";
}

char *itoa(uint64_t n)
{
    return "itoa() IS NOT YET READY\n";
}