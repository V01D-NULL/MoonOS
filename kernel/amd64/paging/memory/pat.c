#include "pat.h"
#include <ctypes.h>
#include <amd64/msr.h>

static uint64_t pat_value = 0;

void configure_pat(void)
{
    // Just assume pat is supported because.. yes.
    auto pat = rdmsr(PAT_MSR);
    uint8_t *pat_arr = (uint8_t*)&pat;
    pat_arr[PA0_UC] = PAT_UC;
    pat_arr[PA1_WC] = PAT_WC;
    pat_arr[PA2_WT] = PAT_WT;
    pat_arr[PA3_WP] = PAT_WP;
    pat_arr[PA4_WB] = PAT_WB;
    pat_arr[PA5_UCM] = PAT_UCM;
    wrmsr(PAT_MSR, pat);
}

void set_pat(uint64_t pat)
{
    pat_value = pat;
}

int get_pat(void)
{
    return pat_value;
}
