#include "pat.h"
#include <ktypes.h>
#include <amd64/msr.h>

static uint64_t pat_value = 0;

void configure_pat(void)
{
    // Just assume pat is supported.
	wrmsr(PAT_MSR, 0x0000000005010406);
}

void set_pat(uint64_t pat)
{
    pat_value = pat;
}

int get_pat(void)
{
    return pat_value;
}
