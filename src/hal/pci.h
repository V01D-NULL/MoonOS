#ifndef PCI_H
#define PCI_H

#include "../common.h"

/*
    PCI Segment groups will NOT be supported
*/

#define PCI_SEGMENT_MAX     65535  //0-65535
#define PCI_BUS_MAX         256    //0-256
#define PCI_SLOT_MAX        31     //0-31
#define PCI_FUNCTION_MAX    7      //0-7

#define CONFIG_ADDR         0xCF8
#define CONFIG_DATA         0xCFC    

#define BAR0_OFFSET         0x10
#define BAR1_OFFSET         0x14
#define BAR2_OFFSET         0x18
#define BAR3_OFFSET         0x1C
#define BAR4_OFFSET         0x20
#define BAR5_OFFSET         0x24

void init_pci();
bool pci_supported();
void pci_check_device(u8int bus, u8int device);
static u16int pci_read_word(u8int bus, u8int slot, u8int func, u8int offset);
static bool does_pci_bus_exsist();
const char* get_pci_vendor_name(u16int vendorID);
const char* get_pci_class_name(u8int base_class);
const char* get_pci_sub_class_name(u8int baseClass, u8int subclass);

u16int get_pci_vendor_id(u8int bus, u8int slot, u8int func);
void pci_check_all_busses();
u16int pci_get_command(u8int bus, u8int device, u8int func);

void PCI_SetCommand(u8int bus, u8int device, u8int function, u16int command);
u16int PCI_GetDeviceID(u8int bus, u8int slot, u8int function);
u16int PCI_GetClassCodes(u8int bus, u8int device, u8int function);
void pci_check_func(u8int bus, u8int device, u8int func, u16int vendorID);

#endif