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
struct idt_desc
{
    uint16_t offset_low;  // offset bits 0..15
    uint16_t selector;    // a code segment selector in GDT or LDT
    uint8_t ist;          // bits 0..2 holds Interrupt Stack Table offset, rest of bits zero.
    uint8_t type_attr;    // type and attributes
    uint16_t offset_mid;  // offset bits 16..31
    uint32_t offset_high; // offset bits 32..63
    uint32_t zero;        // reserved
} gnu_pack_bytes;

/**
 * @brief The values used to load the IDT into memory using 'lidt'
 *
 */
struct idtr
{
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

// CPU exceptions
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

// HW interrupts
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

gnu_export void isr48(void);
gnu_export void isr49(void);
gnu_export void isr50(void);
gnu_export void isr51(void);
gnu_export void isr52(void);
gnu_export void isr53(void);
gnu_export void isr54(void);
gnu_export void isr55(void);
gnu_export void isr56(void);
gnu_export void isr57(void);
gnu_export void isr58(void);
gnu_export void isr59(void);
gnu_export void isr60(void);
gnu_export void isr61(void);
gnu_export void isr62(void);
gnu_export void isr63(void);
gnu_export void isr64(void);
gnu_export void isr65(void);
gnu_export void isr66(void);
gnu_export void isr67(void);
gnu_export void isr68(void);
gnu_export void isr69(void);
gnu_export void isr70(void);
gnu_export void isr71(void);
gnu_export void isr72(void);
gnu_export void isr73(void);
gnu_export void isr74(void);
gnu_export void isr75(void);
gnu_export void isr76(void);
gnu_export void isr77(void);
gnu_export void isr78(void);
gnu_export void isr79(void);
gnu_export void isr80(void);
gnu_export void isr81(void);
gnu_export void isr82(void);
gnu_export void isr83(void);
gnu_export void isr84(void);
gnu_export void isr85(void);
gnu_export void isr86(void);
gnu_export void isr87(void);
gnu_export void isr88(void);
gnu_export void isr89(void);
gnu_export void isr90(void);
gnu_export void isr91(void);
gnu_export void isr92(void);
gnu_export void isr93(void);
gnu_export void isr94(void);
gnu_export void isr95(void);
gnu_export void isr96(void);
gnu_export void isr97(void);
gnu_export void isr98(void);
gnu_export void isr99(void);
gnu_export void isr100(void);
gnu_export void isr101(void);
gnu_export void isr102(void);
gnu_export void isr103(void);
gnu_export void isr104(void);
gnu_export void isr105(void);
gnu_export void isr106(void);
gnu_export void isr107(void);
gnu_export void isr108(void);
gnu_export void isr109(void);
gnu_export void isr110(void);
gnu_export void isr111(void);
gnu_export void isr112(void);
gnu_export void isr113(void);
gnu_export void isr114(void);
gnu_export void isr115(void);
gnu_export void isr116(void);
gnu_export void isr117(void);
gnu_export void isr118(void);
gnu_export void isr119(void);
gnu_export void isr120(void);
gnu_export void isr121(void);
gnu_export void isr122(void);
gnu_export void isr123(void);
gnu_export void isr124(void);
gnu_export void isr125(void);
gnu_export void isr126(void);
gnu_export void isr127(void);
gnu_export void isr128(void);
gnu_export void isr129(void);
gnu_export void isr130(void);
gnu_export void isr131(void);
gnu_export void isr132(void);
gnu_export void isr133(void);
gnu_export void isr134(void);
gnu_export void isr135(void);
gnu_export void isr136(void);
gnu_export void isr137(void);
gnu_export void isr138(void);
gnu_export void isr139(void);
gnu_export void isr140(void);
gnu_export void isr141(void);
gnu_export void isr142(void);
gnu_export void isr143(void);
gnu_export void isr144(void);
gnu_export void isr145(void);
gnu_export void isr146(void);
gnu_export void isr147(void);
gnu_export void isr148(void);
gnu_export void isr149(void);
gnu_export void isr150(void);
gnu_export void isr151(void);
gnu_export void isr152(void);
gnu_export void isr153(void);
gnu_export void isr154(void);
gnu_export void isr155(void);
gnu_export void isr156(void);
gnu_export void isr157(void);
gnu_export void isr158(void);
gnu_export void isr159(void);
gnu_export void isr160(void);
gnu_export void isr161(void);
gnu_export void isr162(void);
gnu_export void isr163(void);
gnu_export void isr164(void);
gnu_export void isr165(void);
gnu_export void isr166(void);
gnu_export void isr167(void);
gnu_export void isr168(void);
gnu_export void isr169(void);
gnu_export void isr170(void);
gnu_export void isr171(void);
gnu_export void isr172(void);
gnu_export void isr173(void);
gnu_export void isr174(void);
gnu_export void isr175(void);
gnu_export void isr176(void);
gnu_export void isr177(void);
gnu_export void isr178(void);
gnu_export void isr179(void);
gnu_export void isr180(void);
gnu_export void isr181(void);
gnu_export void isr182(void);
gnu_export void isr183(void);
gnu_export void isr184(void);
gnu_export void isr185(void);
gnu_export void isr186(void);
gnu_export void isr187(void);
gnu_export void isr188(void);
gnu_export void isr189(void);
gnu_export void isr190(void);
gnu_export void isr191(void);
gnu_export void isr192(void);
gnu_export void isr193(void);
gnu_export void isr194(void);
gnu_export void isr195(void);
gnu_export void isr196(void);
gnu_export void isr197(void);
gnu_export void isr198(void);
gnu_export void isr199(void);
gnu_export void isr200(void);
gnu_export void isr201(void);
gnu_export void isr202(void);
gnu_export void isr203(void);
gnu_export void isr204(void);
gnu_export void isr205(void);
gnu_export void isr206(void);
gnu_export void isr207(void);
gnu_export void isr208(void);
gnu_export void isr209(void);
gnu_export void isr210(void);
gnu_export void isr211(void);
gnu_export void isr212(void);
gnu_export void isr213(void);
gnu_export void isr214(void);
gnu_export void isr215(void);
gnu_export void isr216(void);
gnu_export void isr217(void);
gnu_export void isr218(void);
gnu_export void isr219(void);
gnu_export void isr220(void);
gnu_export void isr221(void);
gnu_export void isr222(void);
gnu_export void isr223(void);
gnu_export void isr224(void);
gnu_export void isr225(void);
gnu_export void isr226(void);
gnu_export void isr227(void);
gnu_export void isr228(void);
gnu_export void isr229(void);
gnu_export void isr230(void);
gnu_export void isr231(void);
gnu_export void isr232(void);
gnu_export void isr233(void);
gnu_export void isr234(void);
gnu_export void isr235(void);
gnu_export void isr236(void);
gnu_export void isr237(void);
gnu_export void isr238(void);
gnu_export void isr239(void);
gnu_export void isr240(void);
gnu_export void isr241(void);
gnu_export void isr242(void);
gnu_export void isr243(void);
gnu_export void isr244(void);
gnu_export void isr245(void);
gnu_export void isr246(void);
gnu_export void isr247(void);
gnu_export void isr248(void);
gnu_export void isr249(void);
gnu_export void isr250(void);
gnu_export void isr251(void);
gnu_export void isr252(void);
gnu_export void isr253(void);
gnu_export void isr254(void);
gnu_export void isr255(void);

#endif // IDT_H