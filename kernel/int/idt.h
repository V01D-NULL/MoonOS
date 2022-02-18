/**
 * @file idt.h
 * @author Tim (V01D)
 * @brief Interrupt Descriptor Table
 * @version 0.1
 * @date 2021-04-16
 * 
 * @copyright Copyright (c) 2021
 * 
 */

#ifndef IDT_H
#define IDT_H

#include <stdint.h>
#include <stddef.h>
#include <util/common.h>
#include <amd64/cpu.h>
#include <amd64/moon.h>

/**
 * @brief Defines an IDT entry
 * 
 */
struct idt_desc {
    uint16_t offset_low;  // offset bits 0..15
    uint16_t selector;    // a code segment selector in GDT or LDT
    uint8_t  ist;         // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
    uint8_t  type_attr;   // type and attributes
    uint16_t offset_mid;  // offset bits 16..31
    uint32_t offset_high; // offset bits 32..63
    uint32_t zero;        // reserved
} gnu_pack_bytes;

/**
 * @brief The values used to load the IDT into memory using 'lidt'
 * 
 */
struct idtr {
    uint16_t limit;
    uint64_t offset;
} gnu_pack_bytes;

/**
 * @brief Set an IDT entry
 * 
 * @param selector  Code segment
 * @param ist       Interrupt stack table offset
 * @param type_attr Types and attributes
 * @param offset    Low, mid, and high offset
 * @param idx       IDT entry index
 */
void idt_set_entry(uint16_t selector, uint8_t ist, uint8_t type_attr, uint64_t offset, uint8_t idx);

/**
 * @brief Setup and load IDT
 * 
 */
void init_idt(void);

//CPU exceptions
gnu_export void isr0(void);
gnu_export void isr1(void);
gnu_export void isr2(void);
gnu_export void isr3(void);
gnu_export void isr4(void);
gnu_export void isr5(void);
gnu_export void isr6(void);
gnu_export void isr7(void);
gnu_export void isr_err8(void);
gnu_export void isr9(void);
gnu_export void isr_err10(void);
gnu_export void isr_err11(void);
gnu_export void isr_err12(void);
gnu_export void isr_err13(void);
gnu_export void isr_err14(void);
gnu_export void isr15(void);
gnu_export void isr16(void);
gnu_export void isr17(void);
gnu_export void isr18(void);
gnu_export void isr19(void);
gnu_export void isr20(void);
gnu_export void isr21(void);
gnu_export void isr22(void);
gnu_export void isr23(void);
gnu_export void isr24(void);
gnu_export void isr25(void);
gnu_export void isr26(void);
gnu_export void isr27(void);
gnu_export void isr28(void);
gnu_export void isr29(void);
gnu_export void isr30(void);
gnu_export void isr31(void);

//HW interrupts
gnu_export void isr32(void);
gnu_export void isr33(void);
gnu_export void isr34(void);
gnu_export void isr35(void);
gnu_export void isr36(void);
gnu_export void isr37(void);
gnu_export void isr38(void);
gnu_export void isr39(void);
gnu_export void isr40(void);
gnu_export void isr41(void);
gnu_export void isr42(void); 
gnu_export void isr43(void); 
gnu_export void isr44(void); 
gnu_export void isr45(void); 
gnu_export void isr46(void); 
gnu_export void isr47(void);

gnu_export void isr128(void);

#endif // IDT_H