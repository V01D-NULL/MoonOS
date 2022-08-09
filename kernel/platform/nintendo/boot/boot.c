#define PR_MODULE "boot"

#include <core/int/interrupt.h>
#include <kernel.h>
#include <printk.h>

void bootstrap_kernel(void)
{
	trace("Welcome to MoonOS\n");
	ivt_init();
	kern_main();
}