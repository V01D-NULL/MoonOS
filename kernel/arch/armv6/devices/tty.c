#include "tty.h"
#include "term/term.h"

void arch_tty_write(string_view str)
{
	_term_write(str, strlen(str));
}
