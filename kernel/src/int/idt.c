#include "idt.h"
#include <stddef.h>

static struct idt_desc idt[256];
// static struct idtr     idtr;

static const char *exception_messages[] = { "Divide by zero",
                                            "Debug",
                                            "NMI",
                                            "Breakpoint",
                                            "Overflow",
                                            "Bound Range Exceeded",
                                            "Invalid Opcode",
                                            "Device Not Available", 
                                            "Double fault", 
                                            "Co-processor Segment Overrun",
                                            "Invaild TSS",
                                            "Segment not present",
                                            "Stack-Segment Fault",
                                            "GPF",
                                            "Page Fault",
                                            "Reserved",
                                            "x87 Floating Point Exception",
                                            "allignement check",
                                            "Machine check",
                                            "SIMD floating-point exception",
                                            "Virtualization Excpetion",
                                            "Deadlock",
                                            "Reserved",
                                            "Reserved",
                                            "Reserved",
                                            "Reserved",
                                            "Reserved",
                                            "Reserved",
                                            "Reserved",
                                            "Reserved",
                                            "Reserved",
                                            "Security Exception",
                                            "Reserved",
                                            "Triple Fault",
                                            "FPU error"
                                         };

void idt_set_entry(uint16_t selector, uint8_t ist, uint8_t type_attr, uint64_t offset /*offset2 + offset3*/, uint8_t idx)
{
    // Offset selection explained:
    //(uint16_t) cast means to get only 16 bits, the bit shift to the right by 16 or 32 specifies the next 16 bits instead of declaring the same offset location over and over
    // Maybe this helps you, I know I struggled with it for long time

    idt[idx] = (struct idt_desc) {
                (uint16_t)offset, // offset low
                selector,
                ist,
                type_attr,
                (uint16_t)(offset >> 16), // offset mid
                (uint32_t)(offset >> 32), // offset high
                0
    };
}

//This should print 'a' when a key is pressed but it doesn't
void handler(struct regs* regs)
{
    kprintf("a");
}

isr_handler_t isr_handlers[256] = {
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL,
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 32
    NULL, handler, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 64
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 96 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 128 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 160
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 192
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 224
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, 
    NULL, NULL, NULL, NULL, NULL, NULL, NULL, NULL, // 256
};

void isr_handler(struct regs* regs)
{
    if (regs->isr_number < 32)
    {
        set_color(VGA_BLACK, VGA_LIGHT_RED);
        serial_set_color(BASH_RED);
        kprintf("[INTR] INT#%d - %s (err_code %d)\n", regs->isr_number, exception_messages[regs->isr_number], regs->error_code);
        debug("INT#%d - %s (err_code %d)\n", regs->isr_number, exception_messages[regs->isr_number], regs->error_code);
        serial_set_color(BASH_WHITE);
        debug("Register dump:\n"                      \
                "rax %x, rbx %x, rcx %x, rdx %x\n"    \
                "rbp %x, rsp %x, rdi %x, rsi %x\n"    \
                "rip %x, cs  %x, ss  %x, rflags %x\n" \
                "r8  %x, r9  %x, r10 %x, r11  %x\n"   \
                "r12 %x, r13 %x, r14 %x, r15  %x\n",
                                                                regs->rax,
                                                                regs->rbx,
                                                                regs->rcx,
                                                                regs->rdx,
                                                                regs->rbp,
                                                                regs->rsp,
                                                                regs->rdi,
                                                                regs->rsi,
                                                                regs->rip,
                                                                regs->cs,
                                                                regs->ss,
                                                                regs->rflags,
                                                                regs->r8,
                                                                regs->r9,
                                                                regs->r10,
                                                                regs->r11,
                                                                regs->r12,
                                                                regs->r13,
                                                                regs->r14,
                                                                regs->r15
                                                                );
        
        set_color(VGA_BLACK, VGA_WHITE);

        asm("hlt");
    }
    
    if (isr_handlers[regs->isr_number] != NULL)
    {
        kprintf("custom irq\n");
        isr_handlers[regs->isr_number](regs);
    }
    else {
        kprintf("Undefined interrupt!\n");
        debug("Undefined interrupt!\n");
    }
    //Write EOI
    outb(0xA0, 0x20);
    outb(0x20, 0x20);
}

void install_isr(int idx, isr_handler_t handler)
{
    isr_handlers[idx] = handler;
}

extern void load_idt(uint64_t idtr);

void init_idt()
{
    monitor_write("Initialising interrupts\n", true, false);
    // idt_set_entry(uint16_t selector /*kernel CS (0x08)*/, uint8_t ist /*0*/, uint8_t type_attr /*0x8E*/, uint64_t offset /*asm func*/, uint8_t idx /*isr index*/)
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr0, 0);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr1, 1);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr2, 2);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr3, 3);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr4, 4);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr5, 5);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr6, 6);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr7, 7);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr_err8, 8);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr9, 9);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr_err10, 10);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr_err11, 11);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr_err12, 12);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr_err13, 13);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr_err14, 14);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr15, 15);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr16, 16);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr17, 17);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr18, 18);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr19, 19);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr20, 20);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr21, 21);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr22, 22);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr23, 23);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr24, 24);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr25, 25);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr26, 26);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr27, 27);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr28, 28);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr29, 29);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr30, 30);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr31, 31);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr32, 32);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr33, 33);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr34, 34);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr35, 35);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr36, 36);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr37, 37);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr38, 38);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr39, 39);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr40, 40);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr41, 41);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr42, 42);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr43, 43);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr44, 44);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr45, 45);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr46, 46);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr47, 47);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr48, 48);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr49, 49);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr50, 50);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr51, 51);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr52, 52);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr53, 53);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr54, 54);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr55, 55);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr56, 56);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr57, 57);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr58, 58);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr59, 59);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr60, 60);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr61, 61);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr62, 62);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr63, 63);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr64, 64);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr65, 65);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr66, 66);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr67, 67);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr68, 68);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr69, 69);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr70, 70);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr71, 71);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr72, 72);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr73, 73);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr74, 74);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr75, 75);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr76, 76);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr77, 77);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr78, 78);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr79, 79);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr80, 80);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr81, 81);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr82, 82);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr83, 83);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr84, 84);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr85, 85);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr86, 86);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr87, 87);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr88, 88);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr89, 89);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr90, 90);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr91, 91);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr92, 92);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr93, 93);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr94, 94);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr95, 95);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr96, 96);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr97, 97);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr98, 98);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr99, 99);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr100, 100);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr101, 101);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr102, 102);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr103, 103);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr104, 104);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr105, 105);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr106, 106);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr107, 107);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr108, 108);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr109, 109);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr110, 110);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr111, 111);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr112, 112);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr113, 113);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr114, 114);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr115, 115);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr116, 116);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr117, 117);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr118, 118);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr119, 119);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr120, 120);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr121, 121);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr122, 122);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr123, 123);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr124, 124);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr125, 125);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr126, 126);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr127, 127);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr128, 128);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr129, 129);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr130, 130);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr131, 131);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr132, 132);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr133, 133);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr134, 134);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr135, 135);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr136, 136);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr137, 137);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr138, 138);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr139, 139);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr140, 140);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr141, 141);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr142, 142);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr143, 143);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr144, 144);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr145, 145);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr146, 146);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr147, 147);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr148, 148);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr149, 149);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr150, 150);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr151, 151);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr152, 152);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr153, 153);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr154, 154);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr155, 155);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr156, 156);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr157, 157);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr158, 158);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr159, 159);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr160, 160);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr161, 161);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr162, 162);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr163, 163);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr164, 164);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr165, 165);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr166, 166);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr167, 167);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr168, 168);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr169, 169);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr170, 170);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr171, 171);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr172, 172);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr173, 173);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr174, 174);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr175, 175);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr176, 176);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr177, 177);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr178, 178);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr179, 179);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr180, 180);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr181, 181);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr182, 182);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr183, 183);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr184, 184);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr185, 185);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr186, 186);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr187, 187);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr188, 188);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr189, 189);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr190, 190);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr191, 191);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr192, 192);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr193, 193);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr194, 194);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr195, 195);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr196, 196);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr197, 197);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr198, 198);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr199, 199);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr200, 200);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr201, 201);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr202, 202);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr203, 203);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr204, 204);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr205, 205);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr206, 206);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr207, 207);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr208, 208);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr209, 209);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr210, 210);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr211, 211);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr212, 212);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr213, 213);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr214, 214);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr215, 215);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr216, 216);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr217, 217);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr218, 218);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr219, 219);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr220, 220);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr221, 221);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr222, 222);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr223, 223);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr224, 224);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr225, 225);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr226, 226);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr227, 227);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr228, 228);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr229, 229);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr230, 230);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr231, 231);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr232, 232);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr233, 233);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr234, 234);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr235, 235);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr236, 236);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr237, 237);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr238, 238);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr239, 239);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr240, 240);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr241, 241);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr242, 242);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr243, 243);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr244, 244);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr245, 245);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr246, 246);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr247, 247);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr248, 248);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr249, 249);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr250, 250);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr251, 251);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr252, 252);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr253, 253);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr254, 254);
    idt_set_entry(0x8, 0, 0x8E, (uint64_t)isr255, 255);
    
    // Load IDT
    struct idtr _idtr = { 
        .limit  = 256 * sizeof(struct idt_desc) - 1, 
        .offset = (uint64_t)idt
    };
    
    load_idt((uint64_t)&_idtr);
    // asm volatile("lidtq %0" :"=m"(_idtr));
    // asm volatile("sti");
    
    monitor_write("Initialised interrupts\n", false, true);
}