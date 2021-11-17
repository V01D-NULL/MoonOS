#include "interrupts.h"
#include "idt.h"
#include <devices/serial/serial.h>
#include <mm/cpu/CR.h>
#include <mm/vmm.h>
#include <amd64/bytes.h>
#include <util/ptr.h>
#include <stddef.h>
#include <stdint.h>
#include <printk.h>

isr_t isr_handler_array[255] = {0};
bool canReturn = false;

static const char *exception_messages[] = {
    "Type: (#DE) Division Exception",
    "Type: (#DB) Debug Exception",
    "Type: (NONE) NMI interrupt (Non Maskable Interrupt)",
    "Type: (#BP) Breakpoint Exception",
    "Type: (#OF) Into detected Overflow Exception",
    "Type: (#BR) BOUND Range Exceeded",
    "Type: (#UD) Invalid Opcode (Undefined Opcode)",
    "Type: (#NM) Device Not Available (No Math Coprocessor)",
    "Type: (#DF) Double Fault",
    "Type: (NONE) Coprocessor Segment Overrun (reserved)",
    "Type: (#TS) Invalid TSS",
    "Type: (#NP) Segment Not Present",
    "Type: (#SS) Stack-Segment Fault",
    "Type: (#GP) General Protection Fault",
    "Type: (#PF) Page Fault",
    "Type: (NONE) Unknown interrupt",
    "Type: (#CF) Coprocessor fault",
    "Type: (#AC) Alignment check",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved",
    "Type: (NONE) Reserved"};

void isr_handler(regs_t regs)
{
    /* CPU exceptions */
    if (regs.isr_number < 32)
    {
        serial_set_color(BASH_RED);
        printk("INT", "%s (err_code %ld)\n", exception_messages[regs.isr_number], regs.error_code);

        if (regs.isr_number == 14)
        {
            printk("INT ~ #PF", "Faulting address: 0x%lx\n", cr_read(CR2));
            uint64_t cr2 = cr_read(CR2);
            vmm_guess_and_map(cr2, regs.error_code);
            return;
        }
        else if (regs.isr_number == 6)
        {
            printk("INT", "Faulting instruction is at: 0x%llx\n", regs.rip);
            printk("INT", "Opcode: %x\n", byte_order_reverse_char8(regs.rip & 0xff));
        }

        debug(true, "INT#%d - %s (err_code %ld)\n", regs.isr_number, exception_messages[regs.isr_number], regs.error_code);
        serial_set_color(BASH_WHITE);
        debug(false, "Register dump:\n"
                     "rax 0x%x, rbx 0x%x, rcx 0x%x, rdx    0x%x\n"
                     "rbp 0x%x, rsp 0x%x, rdi 0x%x, rsi    0x%x\n"
                     "rip 0x%x, cs  0x%x, ss  0x%x, rflags 0x%x\n"
                     "r8  0x%x, r9  0x%x, r10 0x%x, r11    0x%x\n"
                     "r12 0x%x, r13 0x%x, r14 0x%x, r15    0x%x\n",
              regs.rax,
              regs.rbx,
              regs.rcx,
              regs.rdx,
              regs.rbp,
              regs.rsp,
              regs.rdi,
              regs.rsi,
              regs.rip,
              regs.cs,
              regs.ss,
              regs.rflags,
              regs.r8,
              regs.r9,
              regs.r10,
              regs.r11,
              regs.r12,
              regs.r13,
              regs.r14,
              regs.r15);

        if (!canReturn)
        {
            for (;;)
                __asm__("hlt");
        }
    }

    TRY_EXEC_HANDLER(regs.isr_number);

    //Signal EOI
    if (regs.isr_number > 40)
        outb(0xA0, 0x20);
    outb(0x20, 0x20);
}

void install_isr(uint8_t base, isr_t handler)
{
    if (isr_handler_array[base] == 0)
    {
        isr_handler_array[base] = handler;
    }
    else
        printk("INT", "The interrupt ( %d ) has already been registered!\n", base);
}
