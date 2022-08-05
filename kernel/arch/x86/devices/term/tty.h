#ifndef TTY_H
#define TTY_H

#include <base/fmt.h>
#include <base/string.h>
#include "limine-port/term.h"
#include "early/early_term.h"

// #ifndef TTY_BUFF_SZ
// #define TTY_BUFF_SZ 512
// #endif

#define arch_tty_init()                 \
	do                                  \
	{                                   \
		set_boot_term_available(false); \
		term_init();                    \
	} while (0)

void arch_tty_write(string_view str);

#endif // TTY_H