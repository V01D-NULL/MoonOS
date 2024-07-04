#define PR_MODULE "3ds-ivt"

#include <base/base-types.h>
#include <panic.h>
#include <printk.h>

#define FAR_JUMP 0xE51FF004  // LDR PC, [PC, #-4]

extern void irq_handler();
extern void fiq_handler();
extern void svc_handler();
extern void ud_handler();
extern void prefetch_abort_handler();
extern void data_abort_handler();

// clang-format off
uint32_t arm11_ivt[6] = {
	(uint32_t)&irq_handler,
	(uint32_t)&fiq_handler,
	(uint32_t)&svc_handler,
	(uint32_t)&ud_handler,
	(uint32_t)&prefetch_abort_handler,
	(uint32_t)&data_abort_handler
};

string_view str_table[6] = {
	"IRQ",
	"FIQ",
	"SVC",
	"UD",
	"Prefetch abort",
	"Data abort"
};
// clang-format on

NORETURN void a11_exception_main(int arg0)
{
    trace(TRACE_INT, "Error: %s\n", str_table[arg0]);
    for (;;)
        ;
    // panic("ARM11: An exception occurred (%s)", str_table[arg0]);
}

void ivt_init(void)
{
    // The bootrom vectors are stored at the end of AXI WRAM
    // Nintendo's IVT layout must be used until the mmu is enabled.
    volatile uint32_t *arm11_bootrom_vectors = (volatile uint32_t *)0x1FFFFFA0;
    arm11_bootrom_vectors[0]                 = FAR_JUMP;
    arm11_bootrom_vectors[1]                 = arm11_ivt[0];

    arm11_bootrom_vectors[2] = FAR_JUMP;
    arm11_bootrom_vectors[3] = arm11_ivt[1];

    arm11_bootrom_vectors[4] = FAR_JUMP;
    arm11_bootrom_vectors[5] = arm11_ivt[2];

    arm11_bootrom_vectors[6] = FAR_JUMP;
    arm11_bootrom_vectors[7] = arm11_ivt[3];

    arm11_bootrom_vectors[8] = FAR_JUMP;
    arm11_bootrom_vectors[9] = arm11_ivt[4];

    arm11_bootrom_vectors[10] = FAR_JUMP;
    arm11_bootrom_vectors[11] = arm11_ivt[5];
    // trace(TRACE_INT, "Setup bootrom IVT\n");
}