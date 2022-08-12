#define PR_MODULE "boot"

#include <core/int/interrupt.h>
#include <kernel.h>
#include <printk.h>
#include <devices/term/gterm.h>
#include <panic.h>

void bootstrap_kernel(void)
{
	// trace("Welcome to MoonOS\n");
	ivt_init();
	gterm_prepare();
	gterm_init();
	kern_main();
}