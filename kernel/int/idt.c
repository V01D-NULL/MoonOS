#include "idt.h"
#include <stddef.h>
#include <stdint.h>
#include <amd64/moon.h>
#include <util/ptr.h>
#include <hal/pic/pic.h>

static struct idt_desc idt[256];

void idt_set_entry(uint16_t selector, uint8_t ist, uint8_t type_attr, uint64_t offset /*offset2 + offset3*/, uint8_t idx)
{
    idt[idx].offset_low = offset & 0xFFFF;
    idt[idx].selector = selector;
    idt[idx].ist = ist;
    idt[idx].type_attr = type_attr;
    idt[idx].offset_mid = (offset >> 16) & 0xFFFF;
    idt[idx].offset_high = (offset >> 32) & 0xFFFFFFFF;
    idt[idx].zero = 0;
}

gnu_export void load_idt(uint64_t idtr);

void init_idt(void)
{
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr0), 0);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr1), 1);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr2), 2);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr3), 3);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr4), 4);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr5), 5);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr6), 6);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr7), 7);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr_err8), 8);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr9), 9);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr_err10), 10);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr_err11), 11);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr_err12), 12);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr_err13), 13);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr_err14), 14);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr15), 15);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr16), 16);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr17), 17);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr18), 18);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr19), 19);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr20), 20);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr21), 21);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr22), 22);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr23), 23);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr24), 24);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr25), 25);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr26), 26);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr27), 27);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr28), 28);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr29), 29);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr30), 30);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr31), 31);

    //Remap PIC
    pic_remap();

    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr32), 32);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr33), 33);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr34), 34);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr35), 35);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr36), 36);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr37), 37);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr38), 38);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr39), 39);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr40), 40);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr41), 41);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr42), 42);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr43), 43);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr44), 44);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr45), 45);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr46), 46);
    idt_set_entry(0x28, 0, 0x8E, GENERIC_CAST(uint64_t, isr47), 47);

    // Load IDT
    struct idtr _idtr = {
        .limit = 256 * sizeof(struct idt_desc) - 1,
        .offset = (uint64_t)idt
    };

    load_idt(GENERIC_CAST(uint64_t, &_idtr));
}
