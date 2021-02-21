#include "usermode.h"

void enter_usermode()
{

}

// Prints the Current Privilege Level
void get_cpl()
{
    /*
    *   The CPL is stored in bits 0 & 1 of cs and ss
    */

    int cs;
    int ss;

    asm volatile("movl %%cs, %0" : "=r"(cs));
    asm volatile("movl %%ss, %0" : "=r"(ss));

    debug("CPL_CS: %x %b\n", cs, cs);
    debug("CPL_SS: %x %b\n", ss, ss);

    int is_cs_0_set = CHECK_BIT(cs, 0);
    int is_cs_1_set = CHECK_BIT(cs, 1);

    int is_ss_0_set = CHECK_BIT(ss, 0);
    int is_ss_1_set = CHECK_BIT(ss, 1);
    
    debug("CS_BIT0: %d\nCS_BIT1: %d\nSS_BIT_0: %d\nSS_BIT_1: %d\n", is_cs_0_set, is_cs_1_set, is_ss_0_set, is_ss_1_set);
    if (is_cs_0_set == 0 && is_cs_1_set == 0 && is_ss_0_set == 0 && is_ss_1_set == 0)
    {
        monitor_write("OS is running in supervisor mode\n", false, true);
    }
}