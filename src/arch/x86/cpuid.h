#ifndef CPUID_H
#define CPUID_H

#include "../../common.h"
#include "../../drivers/screen/monitor.h"


int get_cpu_model_number(void);
char* get_vendor_string();
void checkCPU();

#endif