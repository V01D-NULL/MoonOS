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
} __pack_bytes;

/**
 * @brief The values used to load the IDT into memory using 'lidt'
 * 
 */
struct idtr {
    uint16_t limit;
    uint64_t offset;
} __pack_bytes;

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
__export void isr0(void);
__export void isr1(void);
__export void isr2(void);
__export void isr3(void);
__export void isr4(void);
__export void isr5(void);
__export void isr6(void);
__export void isr7(void);
__export void isr_err8(void);
__export void isr9(void);
__export void isr_err10(void);
__export void isr_err11(void);
__export void isr_err12(void);
__export void isr_err13(void);
__export void isr_err14(void);
__export void isr15(void);
__export void isr16(void);
__export void isr17(void);
__export void isr18(void);
__export void isr19(void);
__export void isr20(void);
__export void isr21(void);
__export void isr22(void);
__export void isr23(void);
__export void isr24(void);
__export void isr25(void);
__export void isr26(void);
__export void isr27(void);
__export void isr28(void);
__export void isr29(void);
__export void isr30(void);
__export void isr31(void);

//HW interrupts
__export void isr32(void);
__export void isr33(void);
__export void isr34(void);
__export void isr35(void);
__export void isr36(void);
__export void isr37(void);
__export void isr38(void);
__export void isr39(void);
__export void isr40(void);
__export void isr41(void);
__export void isr42(void); 
__export void isr43(void); 
__export void isr44(void); 
__export void isr45(void); 
__export void isr46(void); 
__export void isr47(void);

#endif // IDT_H