/**
 * PCI Driver
 *
 * NOTE: This was originally taken from a project called 'MyOS' written by
 * codeTrevor. I didn't write this driver and all rights go to the user who
 * wrote this.
 * (Minor adjustments and custom code has been added to this source file)
 */

#include "pci.h"
#include "../drivers/screen/monitor.h"

//Assume pci isn't available by default
bool is_pci_supported = false;

bool pci_supported() {
    return is_pci_supported;
}

//Returns 0 if a bus is found, and -1 if none is found
extern u32int find_pci_bus();

//Returns true if the pci bus was found, otherwise it returns false
static bool does_pci_bus_exsist() {
    u32int pci_search_result = find_pci_bus();
    
    if (pci_search_result == 0) {
        monitor_write("Found PCI bus\n", 0, 1);
        is_pci_supported = true; //PCI bus found, external periphirals are supported
        return true;
    }
    
    else {
        kprintf("Warning! No PCI bus was found! ( Return value of find_pci_bus(): %d )\n", pci_search_result);
        return false;
    }
}

static u16int pci_read_word(u8int bus, u8int slot, u8int func, u8int offset)
{
    u32int addr;
    u32int lbus  = (u32int)bus;
    u32int lslot = (u32int)slot;
    u32int lfunc = (u32int)func;
    u16int tmp   = 0;

    addr = (u32int)((lbus << 16) | (lslot << 11) | (lfunc << 8) | (offset & 0xfc) | ((u32int)0x80000000));
 
    outl(CONFIG_ADDR, addr);
    tmp = (u16int)((inl(0xCFC) >> ((offset & 2) * 8)) & 0xffff);
    return tmp;
}

static void pci_write_dword(u8int bus, u8int slot, u8int func, u8int offset, u32int data) {
    u32int address;
    u32int lbus = (u32int)bus;
    u32int lslot = (u32int)slot;
    u32int lfunc = (u32int)func;

    /* create configuration address as per Figure 1 */
    address = (u32int)((lbus << 16) | (lslot << 11) | (lfunc << 8) |
                         (offset & 0xfc) | ((u32int)0x80000000));

    /* write out the address */
    outl(address, CONFIG_ADDR);

    /* write out the data */
    outl(data, CONFIG_DATA);
}

static u32int pci_read_dword(u8int bus, u8int slot, u8int func, u8int offset) {
    u32int address;
    u32int lbus = (u32int)bus;
    u32int lslot = (u32int)slot;
    u32int lfunc = (u32int)func;

    /* create configuration address */
    address = (u32int)((lbus << 16) | (lslot << 11) | (lfunc << 8) |
                         (offset & 0xfc) | ((u32int)0x80000000));

    /* write out the address */
    outl(address, CONFIG_ADDR);

    /* read in the data */
    return inl(CONFIG_DATA);
}

u8int PCI_GetHeaderType(u8int bus, u8int slot, u8int function) {
    // Header type will be the lower 8 bytes starting at offset 0x0E
    return pci_read_word(bus, slot, function, 0x0E) & 0xFF;
}

u8int PCI_GetInterruptLine(u8int bus, u8int slot, u8int function) {
    return pci_read_word(bus, slot, function, 0x3C) & 0xFF;
}

u8int PCI_GetProgrammingInterface(u8int bus, u8int slot, u8int function) {
    // PROG_IF is upper byte of 16 bits after revision ID
    return (u8int)( pci_read_word(bus, slot, function, 0 ) >> 8);
}

u16int pci_get_command(u8int bus, u8int device, u8int func) {
    return pci_read_word(bus, device, func, 0x04);
}



const char* get_pci_vendor_name(u16int vendorID) {
    switch (vendorID) {
        case 0x1022:
            return "AMD";
        case 0x106B:
            return "Apple Inc.";
        case 0x10EC:
            return "Realtek";
        case 0x1AF4:
            return "Red Hat, Inc.";
        case 0x1234:
            return "QEMU";
        case 0x15AD:
            return "VMWare";
        case 0x80ee:
            return "VirtualBox";
        case 0x8086:
            return "Intel";
    }

    return "Unknown PCI vendor!";
}

const char* get_pci_class_name(u8int base_class)
{
    switch (base_class) {
        case 0x00:
            return "Unclassified";
        case 0x01:
            return "Mass Storage Controller";
        case 0x02:
            return "Network Controller";
        case 0x03:
            return "Display Controller";
        case 0x04:
            return "Multimedia Controller";
        case 0x05:
            return "Memory Controller";
        case 0x06:
            return "Bridge Device";
        case 0x07:
            return "Simple Communication Controller";
        case 0x08:
            return "Base System Peripheral";
        case 0x09:
            return "Input Device Controller";
        case 0x0A:
            return "Docking Station";
        case 0x0B:
            return "Processor";
        case 0x0C:
            return "Serial Bus Controller";
        case 0x0D:
            return "Wireless Controller";
        case 0x0E:
            return "Intelligent Controller";
        case 0x0F:
            return "Satellite Communication Controller";
        case 0x10:
            return "Encryption Controller";
        case 0x11:
            return "Signal Processing Controller";
        case 0x12:
            return "Processing Accelerator";
        case 0x13:
            return "Non-Essential Instrumentation";
        case 0x40:
            return "Co-Processor";
        case 0xFF:
            return "Unassigned Class";
    }
    return "Unknown PCI class";
}


const char* get_pci_sub_class_name(u8int baseClass, u8int subclass) {
    // from https://wiki.osdev.org/PCI
    switch (baseClass) {
        case 0x00:
            // unclassified
            if (subclass == 0)
                return "Non-VGA-Compatible";
            if (subclass == 1)
                return "VGA-Compatible";
            break;
        case 0x01:
            // mass storage controller
            if (subclass == 0)
                return "SCSI Bus Controller";
            if (subclass == 1)
                return "IDE Controller";
            if (subclass == 2)
                return "Floppy Disk Controller";
            if (subclass == 3)
                return "IPI Bus Controller";
            if (subclass == 4)
                return "RAID Controller";
            if (subclass == 5)
                return "ATA Controller";
            if (subclass == 6)
                return "Serial ATA";
            if (subclass == 7)
                return "Serial Attached SCSI";
            if (subclass == 8)
                return "Non-Volatile Memory Controller";
            if (subclass == 0x80)
                return "\"Other\"";
            break;
        case 0x02:
            // Network Controller
            switch (subclass) {
                case 0x00:
                    return "Ethernet Controller";
                case 0x01:
                    return "Token Ring Controller";
                case 0x02:
                    return "FDDI Controller";
                case 0x03:
                    return "ATM Controller";
                case 0x04:
                    return "ISDN Controller";
                case 0x05:
                    return "WorldFip Controller";
                case 0x06:
                    return "PICMG 2.14 Multi Computing";
                case 0x07:
                    return "Infiniband Controller";
                case 0x08:
                    return "Fabric Controller";
                case 0x80:
                    return "\"Other\"";
            }
            break;
        case 0x03:
            // Display Controller
            if (subclass == 0x00)
                return "VGA Compatible Controller";
            if (subclass == 0x01)
                return "XGA Controller";
            if (subclass == 0x02)
                return "3D Controller(Not VGA - Compatible)";
            if (subclass == 0x80)
                return "\"Other\"";
            break;
        case 0x04:
            // Multimedia Controller
            switch (subclass) {
                case 0x00:
                    return "Multimedia Video Controller";
                case 0x01:
                    return "Multimedia Audio Controller";
                case 0x02:
                    return "Computer Telephony Device";
                case 0x03:
                    return "Audio Device";
                case 0x80:
                    return "\"Other\"";
            }
            break;
        case 0x05:
            // Memory Controller
            if (subclass == 0x00)
                return "RAM Controller";
            if (subclass == 0x01)
                return "Flash Controller";
            if (subclass == 0x80)
                return "\"Other\"";
            break;
        case 0x06:
            // Bridge Device
            switch (subclass) {
                case 0x00:
                    return "Host Bridge";
                case 0x01:
                    return "ISA Bridge";
                case 0x02:
                    return "EISA Bridge";
                case 0x03:
                    return "MCA Bridge";
                case 0x04:
                    return "PCI - to - PCI Bridge";
                case 0x05:
                    return "PCMCIA Bridge";
                case 0x06:
                    return "NuBus Bridge";
                case 0x07:
                    return "CardBus Bridge";
                case 0x08:
                    return "RACEway Bridge";
                case 0x09:
                    return "PCI - to - PCI Bridge";
                case 0x0A:
                    return "InfiniBand - to - PCI Host Bridge";
                case 0x80:
                    return "\"Other\"";
            }
            break;
        case 0x07:
            // Simple Communication Controller
            switch (subclass) {
                case 0x00:
                    return "Serial Controller";
                case 0x01:
                    return "Parallel Controller";
                case 0x02:
                    return "Multiport Serial Controller";
                case 0x03:
                    return "Modem";
                case 0x04:
                    return "IEEE 488.1 / 2 (GPIB)Controller";
                case 0x05:
                    return "Smart Card";
                case 0x80:
                    return "\"Other\"";
            }
            break;
        case 0x08:
            // Base System Peripheral
            switch (subclass) {
                case 0x00:
                    return "PIC";
                case 0x01:
                    return "DMA Controller";
                case 0x02:
                    return "Timer";
                case 0x03:
                    return "RTC Controller";
                case 0x04:
                    return "PCI Hot-Plug Controller";
                case 0x05:
                    return "SD Host Controller";
                case 0x06:
                    return "IOMMU";
                case 0x80:
                    return "\"Other\"";
            }
            break;
        case 0x09:
            // Input Device Controller
            switch (subclass) {
                case 0x00:
                    return "Keyboard Controller";
                case 0x01:
                    return "Digitizer Pen";
                case 0x02:
                    return "Mouse Controller";
                case 0x03:
                    return "Scanner Controller";
                case 0x04:
                    return "Gameport Controller";
                case 0x80:
                    return "\"Other\"";
            }
            break;
        case 0x0A:
            // Docking Station
            switch (subclass) {
                case 0x00:
                    return "Generic";
                case 0x80:
                    return "\"Other\"";
            }
            break;
        case 0x0B:
            // Processor
            switch (subclass) {
                case 0x00:
                    return "386";
                case 0x01:
                    return "486";
                case 0x02:
                    return "Pentium";
                case 0x10:
                    return "Alpha";
                case 0x20:
                    return "PowerPC";
                case 0x30:
                    return "MIPS";
                case 0x40:
                    return "Co-Processor";
                case 0x80:
                    return "\"Other\"";
            }
            break;
        case 0x0C:
            // Serial Bus Controller
            switch (subclass) {
                case 0x00:
                    return "FireWire (IEEE 1394) Controller";
                case 0x01:
                    return "ACCESS Bus";
                case 0x02:
                    return "SSA";
                case 0x03:
                    return "USB Controller";
                case 0x04:
                    return "Fibre Channel";
                case 0x05:
                    return "SMBus";
                case 0x06:
                    return "InfiniBand";
                case 0x07:
                    return "IPMI Interface";
                case 0x08:
                    return "SERCOS Interface (IEC 61491)";
                case 0x09:
                    return "CANbus";
            }
            break;
        case 0x0D:
            // Wireless Controller
            switch (subclass) {
                case 0x00:
                    return "iRDA Compatible Controller";
                case 0x01:
                    return "Consumer IR Controller";
                case 0x10:
                    return "RF Controller";
                case 0x11:
                    return "Bluetooth Controller";
                case 0x12:
                    return "Broadband Controller";
                case 0x20:
                    return "Ethernet Controller (802.1a)";
                case 0x21:
                    return "Ethernet Controller (802.1b)";
                case 0x80:
                    return "\"Other\"";
            }
            break;
        case 0x0E:
            // Intelligent Controller
            if (subclass == 0)
                return "I20";
            break;
        case 0x0F:
            // Satellite Communication Controller
            switch (subclass) {
                case 0x01:
                    return "Satellite TV Controller";
                case 0x02:
                    return "Satellite Audio Controller";
                case 0x03:
                    return "Satellite Voice Controller";
                case 0x04:
                    return "Satellite Data Controller";
            }
            break;
        case 0x10:
            // Encryption Controller
            switch (subclass) {
                case 0x00:
                    return "Network and Computing Encrpytion/Decryption";
                case 0x10:
                    return "Entertainment Encryption/Decryption";
                case 0x80:
                    return "Other Encryption/Decryption";
            }
            break;
        case 0x11:
            // Signal Processing Controller
            switch (subclass) {
                case 0x00:
                    return "DPIO Modules";
                case 0x01:
                    return "Performance Counters";
                case 0x10:
                    return "Communication Synchronizer";
                case 0x20:
                    return "Signal Processing Management";
                case 0x80:
                    return "\"Other\"";
            }
            break;
        case 0x12:
            // Processing Accelerator
            return "";
            break;
        case 0x13:
            // Non-Essential Instrumentation
            return "";
            break;
        case 0x40:
            // Co-Processor
            return "";
            break;
    }

    return "[Unknown PCI subclass]";
}

u16int get_pci_vendor_id(u8int bus, u8int slot, u8int func) {
    return pci_read_word(bus, slot, func, 0);
}

void pci_check_all_busses()
{
    u16int bus;
    u8int device;
    for (bus = 0; bus < 256; bus++)
    {
        for (device = 0; device < 32; device++)
        {
            pci_check_device(bus, device);
        }
    }
}

void pci_check_device(u8int bus, u8int device) {
    u8int func = 0;
    
    u16int vendorID = get_pci_vendor_id(bus, device, 0);

    if (vendorID == 0xFFFF) //No such device 
        return;

    op_ok();
    kprintf("Found PCI device -> DevID: %d/%x ( %s )\n", vendorID, vendorID, get_pci_vendor_name(vendorID));

    pci_check_func(bus, device, func, vendorID);

    u8int header_type = PCI_GetHeaderType(bus, device, vendorID);

    if (header_type & 0x80) {
        // It is a multi-function device, so check remaining functions
        for (func = 1; func < 8; ++func) {
            vendorID = get_pci_vendor_id(bus, device, func);
            if (vendorID != 0xFFFF) {
                pci_check_func(bus, device, func, vendorID);
            }
        }
    }
}

void pci_check_func(u8int bus, u8int device, u8int func, u16int vendorID)
{
    u8int base_class, subclass = 0;

    u16int classes = PCI_GetClassCodes(bus, device, func);
    u16int deviceID = PCI_GetDeviceID(bus, device, func);

    // base class is the upper 8 bits
    base_class = (classes & 0xFF00) >> 8;

    // subclass is the lower 8 bits
    subclass = classes & 0xFF;

    // print string from the base class and subclass
    op_ok();
    kprintf("Function: %d - %x - %s\n", func, deviceID, get_pci_sub_class_name(base_class, subclass));

}

void PCI_EnableBusMastering(u8int bus, u8int slot, u8int function) {
    // read in the old command word
    u16int command = pci_read_word(bus, slot, function, 0x04);

    // ensure the bus mastering bit is set
    command |= 0x04;

    // write out the command word
    PCI_SetCommand(bus, slot, function, command);
}

u32int PCI_GetBAR(u8int bus, u8int slot, u8int function, u8int bar) {
    u8int offset = BAR0_OFFSET + (4 * bar);
    return pci_read_word(bus, slot, function, offset);
}

u32int PCI_GetBaseAddress0(u8int bus, u8int slot, u8int function) {
    return pci_read_word(bus, slot, function, BAR0_OFFSET);
}

u32int PCI_GetBaseAddress1(u8int bus, u8int slot, u8int function) {
    return pci_read_word(bus, slot, function, BAR1_OFFSET);
}

u32int PCI_GetBaseAddress2(u8int bus, u8int slot, u8int function) {
    return pci_read_word(bus, slot, function, BAR2_OFFSET);
}

u32int PCI_GetBaseAddress3(u8int bus, u8int slot, u8int function) {
    return pci_read_word(bus, slot, function, BAR3_OFFSET);
}

u32int PCI_GetBaseAddress4(u8int bus, u8int slot, u8int function) {
    return pci_read_word(bus, slot, function, BAR4_OFFSET);
}

u32int PCI_GetBaseAddress5(u8int bus, u8int slot, u8int function) {
    return pci_read_word(bus, slot, function, BAR5_OFFSET);
}

u8int PCI_GetCapabilitiesPointer(u8int bus, u8int slot,
                                   u8int function) {
    u16int pointer =
        pci_read_word(bus, slot, function, 0x34) & ~3;
    return pointer & 0xFF;
}

u16int PCI_GetClassCodes(u8int bus, u8int device, u8int function) {
    return pci_read_word(bus, device, function, 0x0A);
}

u16int PCI_GetDeviceID(u8int bus, u8int slot, u8int function) {
    return pci_read_word(bus, slot, function, 0x02);
}

void PCI_SetCommand(u8int bus, u8int device, u8int function, u16int command) {
    // read in the u32int associated with command (command and status)
    u32int dataDWORD =
        pci_read_dword(bus, device, function, 0x04);

    // modify the command part of the data, overwriting the old command
    dataDWORD = (dataDWORD & 0xFFFF0000) | command;

    // write out the new dword
    pci_write_dword(bus, device, function, 0x04, dataDWORD);
}

void init_pci() {
    monitor_write("Setting up PCI devices...\n", true, false);
 
    //There is no point in continuing PCI operations as no device was found.
    if (!does_pci_bus_exsist()) {
        is_pci_supported = false;
        return;
    }
    pci_check_all_busses();
}