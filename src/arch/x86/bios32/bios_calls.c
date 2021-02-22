#include "bios_calls.h"

void BIOS_shutdown()
{
    debug("Attempting to shutdown computer via unconvential means (BIOS interrupt 0x15)\n");
    //Attempt to shutdown using BIOS
	regs16_t r;
	r.ax = 0x1000;
	asm volatile("movl %%ss, %0" :: "r"(&r.ax));
	r.sp = 0xf000;
	r.ax = 0x5307;
	r.bx = 0x0001;
	r.cx = 0x0003;
	int32(0x15, &r);
}

