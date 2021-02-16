#ifndef CMOS_H
#define CMOS_H

#define CURRENT_YEAR 2021

#include "../common.h"
#include "../drivers/screen/monitor.h"
#include "../arch/x86/cpu/interrupts/interrupts.h"

struct CMOS {
    u32int m_cmos_addr;
    u32int m_cmos_data;
};

extern struct CMOS* cmos;

void init_cmos();
u32int get_update_process_flag();
void read_rtc();
u32int get_date();

#endif