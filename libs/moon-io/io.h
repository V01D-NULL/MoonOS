#ifndef PORT_IO_H
#define PORT_IO_H

#if defined(__x86_64__)

#include <base/base-types.h>

inline void outb(uint16_t port, uint8_t val)
{
	asm volatile("outb %0, %1"
				 :
				 : "a"(val), "Nd"(port));
}

inline void outw(uint16_t port, uint16_t val)
{
	asm volatile("outw %0, %1"
				 :
				 : "a"(val), "Nd"(port));
}

inline void outl(uint16_t port, uint32_t val)
{
	asm volatile("outl %0, %1"
				 :
				 : "a"(val), "Nd"(port));
}

inline uint8_t inb(uint16_t port)
{
	uint8_t ret;
	asm volatile("inb %1, %0"
				 : "=a"(ret)
				 : "Nd"(port));
	return ret;
}

inline uint16_t inw(uint16_t port)
{
	uint16_t ret;
	asm volatile("inw %1, %0"
				 : "=a"(ret)
				 : "Nd"(port));
	return ret;
}

inline uint32_t inl(uint16_t port)
{
	uint32_t ret;
	asm volatile("inb %1, %0"
				 : "=a"(ret)
				 : "Nd"(port));
	return ret;
}

inline void io_wait(void)
{
	inb(0x80);
}

#endif // __x86_64__

inline void mmio_write8(uint32_t io_port, uint8_t val)
{
	*((volatile uint8_t *)io_port) = val;
}

inline void mmio_write16(uint32_t io_port, uint16_t val)
{
	*((volatile uint16_t *)io_port) = val;
}

inline void mmio_write32(uint32_t io_port, uint32_t val)
{
	*((volatile uint32_t *)io_port) = val;
}

inline uint32_t mmio_read(uint32_t io_port)
{
	return *((volatile uint32_t *)io_port);
}

#endif // PORT_IO_H