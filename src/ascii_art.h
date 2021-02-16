#ifndef ASCII_ART_H
#define ASCII_ART_H

#include "drivers/io/serial.h"
#include "drivers/screen/monitor.h"

const char* p1 = " _  _   __   __    __  ____  __  ____  _  _     __   ____\n";
const char* p2 = "/ )( \\ / _\\ (  )  (  )(    \\(  )(_  _)( \\/ )   /  \\ / ___)\n";
const char* p3 = "\\ \\/ //    \\/ (_/\\ )(  ) D ( )(   )(   )  /   (  O )\\___ \\ \n";
const char* p4 = " \\__/ \\_/\\_/\\____/(__)(____/(__) (__) (__/     \\__/ (____/\n";

void banner() {
    serial_write_str(p1);
    serial_write_str(p2);
    serial_write_str(p3);
    serial_write_str(p4);


    for (int i = 0; i < strlen(p1); i++)
    {
        set_color(VGA_BLACK, VGA_DARK_GREY);
        monitor_put(p1[i]);
    }

    for (int i = 0; i < strlen(p2); i++)
    {
        set_color(VGA_BLACK, VGA_LIGHT_GREY);
        monitor_put(p2[i]);
    }

    for (int i = 0; i < strlen(p3); i++)
    {
        set_color(VGA_BLACK, VGA_WHITE);
        monitor_put(p3[i]);
    }

    for (int i = 0; i < strlen(p4); i++)
    {
        set_color(VGA_BLACK, VGA_LIGHT_BLUE);
        monitor_put(p4[i]);
    }
    
    set_color(VGA_BLACK, VGA_WHITE);
    delay(300);
}

#endif // ASCII_ART_H