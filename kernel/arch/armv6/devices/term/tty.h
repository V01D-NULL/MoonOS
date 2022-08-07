#ifndef TTY_H
#define TTY_H

#include <base/fmt.h>
#include <base/string.h>

#define arch_tty_init()   \
	do                    \
	{                     \
		/*	term_init();*/ \
	} while (0)

void arch_tty_write(string_view str);

#endif // TTY_H