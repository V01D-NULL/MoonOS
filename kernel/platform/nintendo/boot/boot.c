#define PR_MODULE "boot"

#include <core/int/interrupt.h>
#include <devices/term/gterm.h>
#include <kernel.h>
#include <panic.h>
#include <printk.h>

void bootstrap_kernel(void)
{
    // trace(TRACE_BOOT, "Welcome to MoonOS\n");
    ivt_init();
    gterm_prepare();
    gterm_init();
    kern_main((HandoverModules){0});
}