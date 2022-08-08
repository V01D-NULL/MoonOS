#include "tty.h"

void arch_tty_write(string_view str)
{
	if (is_boot_term_available())
		boot_term_write("%s", str);
	else
		_term_write(str, strlen(str));
}
