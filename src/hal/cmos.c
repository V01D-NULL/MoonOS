#include "cmos.h"

u8int  second;
u8int  minute;
u8int  hour;
u8int  day;
u8int  month;
u32int year;

u32int century_register = 0x0; 

struct CMOS* cmos;

u32int get_update_process_flag() {
    outb(cmos->m_cmos_addr, 0x0A);
    return (inb(cmos->m_cmos_data) & 0x80);
}

u8int get_rtc_register(int _register) {
    outb(cmos->m_cmos_addr, _register);
    return (inb(cmos->m_cmos_data));
}

void read_rtc()
{
    u8int century;
    u8int last_second;
    u8int last_minute;
    u8int last_hour;
    u8int last_day;
    u8int last_month;
    u8int last_year;
    u8int last_century;
    u8int registerB;

    while(get_update_process_flag()); //Make sure no updates are being made

    second = get_rtc_register(0x00);
    minute = get_rtc_register(0x02);
    hour   = get_rtc_register(0x04);
    day    = get_rtc_register(0x07);
    month  = get_rtc_register(0x08);
    year   = get_rtc_register(0x09);

    if (century_register != 0)
        century = get_rtc_register(century_register);

    do {
        last_second = second;
        last_minute = minute;
        last_hour = hour;
        last_day = day;
        last_month = month;
        last_year = year;
        last_century = century;

        while(get_update_process_flag());
        second = get_rtc_register(0x00);
        minute = get_rtc_register(0x02);
        hour   = get_rtc_register(0x04);
        day    = get_rtc_register(0x07);
        month  = get_rtc_register(0x08);
        year   = get_rtc_register(0x09);

        if (century_register != 0)
            century = get_rtc_register(century_register);
    } while( (last_second != second) || (last_minute != minute) || (last_hour != hour) ||
               (last_day != day) || (last_month != month) || (last_year != year) ||
               (last_century != century) );
    
    registerB = get_rtc_register(0x0B);
    if (!(registerB & 0x04)) {
            second = (second & 0x0F) + ((second / 16) * 10);
            minute = (minute & 0x0F) + ((minute / 16) * 10);
            hour = ( (hour & 0x0F) + (((hour & 0x70) / 16) * 10) ) | (hour & 0x80);
            day = (day & 0x0F) + ((day / 16) * 10);
            month = (month & 0x0F) + ((month / 16) * 10);
            year = (year & 0x0F) + ((year / 16) * 10);

            if(century_register != 0)
                century = (century & 0x0F) + ((century / 16) * 10);
      }

    if (!(registerB & 0x02) && (hour & 0x80)) {
            hour = ((hour & 0x7F) + 12) % 24;
      }
 
      // Calculate the full (4-digit) year
 
      if(century_register != 0) {
            year += century * 100;
      } else {
            year += (CURRENT_YEAR / 100) * 100;
            if(year < CURRENT_YEAR) year += 100;
      }
}

u32int get_date() {
    read_rtc();
    op_ok();
    kprintf_x("Validity: %d:%d:%d %d/%d/%d", hour, minute, second, day, month, year);
}

void init_cmos() {
    monitor_write("Setting up CMOS...\n", true, false);
    cmos->m_cmos_addr = 0x70;
    cmos->m_cmos_data = 0x71;
    read_rtc();
}