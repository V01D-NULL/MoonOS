#include "i2c.h"
#include <base/base-types.h>
#include <moon-io/io.h>

#define GET_CNT_REGISTER(i2c_base) (uint8_t *)(i2c_base + CNT_REG_OFF)
#define IS_BUSY(i2c_base) (*GET_CNT_REGISTER(i2c_base) & CNT_CHECK_AVAILABILITY)

enum I2C_BASE
{
	I2C1_BASE = 0x10161000,
	I2C2_BASE = 0x10144000,
	I2C3_BASE = 0x10148000
};

enum I2C_CNT
{
	CNT_STOP = (1 << 0),			  // 1 = Stop data transfer (last byte), 0 = no data transfer active
	CNT_START = (1 << 1),			  // 1 = Start data transfer (first byte), 0 = no data transfer active
	CNT_PAUSE = (1 << 2),			  // 1 = Pause after Error 0=Transfer Data
	CNT_ACK = (1 << 4),				  // 1 = Acknowledged, 0 = Not acknowledged
	CNT_READ = (1 << 5),			  // 1 = read, 0 = write
	CNT_INT_ENABLE = (1 << 6),		  // 1 = enable interrupts, 0 = disable interrupts
	CNT_CHECK_AVAILABILITY = (1 << 7) // 1 = busy serving another request, 0 = available
};

enum I2C_REGISTER_OFFSETS
{
	DATA_REG_OFF,
	CNT_REG_OFF,
	CNTEX_REG_OFF,
	SCL_REG_OFF = 4
};

// https://www.3dbrew.org/wiki/I2C_Registers#I2C_Devices
struct
{
	uint8_t device_bus_id;
	uint16_t device_register;
} i2c_device_id_table_contents[15] = {
	{0, 0x4A},
	{0, 0x7A},
	{0, 0x78},
	{1, 0x4A},
	{1, 0x78},
	{1, 0x2C},
	{1, 0x2E},
	{1, 0x40},
	{1, 0x44},
	{2, 0xD6},
	{2, 0xD0},
	{2, 0xD2},
	{2, 0xA4},
	{2, 0x9A},
	{2, 0xA0},
};

const inline typeof(i2c_device_id_table_contents[0]) i2c_get_device_id_table_pair(uint8_t device_id)
{
	return i2c_device_id_table_contents[device_id];
}

const inline uint8_t i2c_get_bus_id(uint8_t device_id)
{
	return i2c_get_device_id_table_pair(device_id).device_bus_id;
}

const inline uint8_t i2c_get_device_register(uint8_t device_id)
{
	return i2c_get_device_id_table_pair(device_id).device_register;
}

// Wait for any data transfers to complete
inline void i2c_wait(enum I2C_BASE i2c_base)
{
	do
	{
		asm("nop");
	} while (IS_BUSY(i2c_base));
}

// Write some data into the i2c's data register
inline void i2c_write_data_reg(uint32_t i2c_base, uint8_t data)
{
	mmio_write8(i2c_base, data);
}

// Write a byte into the i2c's CNT register
inline void i2c_write_cnt_reg(uint32_t i2c_base, uint8_t mask)
{
	mmio_write8(i2c_base + CNT_REG_OFF, mask);
}

// Returns true if a command has been ACK'ed, else false (NACK)
inline bool i2c_was_acknowledged(enum I2C_BASE i2c_base)
{
	i2c_wait(i2c_base);
	return (*GET_CNT_REGISTER(i2c_base) >> 4) & 1; // Bit 4: ACK
}

// Called when a NACK was received instead of an ACK.
// The data transfer will be stopped
static inline void i2c_nack(uint32_t i2c_base)
{
	i2c_write_cnt_reg(i2c_base, CNT_CHECK_AVAILABILITY | CNT_INT_ENABLE | CNT_PAUSE | CNT_STOP);
	i2c_wait(i2c_base);
}

static bool try_get_device(uint32_t i2c_base, uint8_t device_register)
{
	i2c_wait(i2c_base);
	i2c_write_data_reg(i2c_base, device_register);
	i2c_write_cnt_reg(i2c_base, CNT_CHECK_AVAILABILITY | CNT_INT_ENABLE | CNT_START);

	return i2c_was_acknowledged(i2c_base);
}

static bool try_get_register(uint32_t i2c_base, uint8_t reg)
{
	i2c_wait(i2c_base);
	i2c_write_data_reg(i2c_base, reg);
	i2c_write_cnt_reg(i2c_base, CNT_CHECK_AVAILABILITY | CNT_INT_ENABLE);

	return i2c_was_acknowledged(i2c_base);
}

static uint32_t i2c_bus_id_to_addr(uint8_t bus_id)
{
	switch (bus_id)
	{
	case 2:
		return I2C3_BASE;
	case 1:
		return I2C2_BASE;
	case 0:
		return I2C1_BASE;

	default:
		return 0;
	}
	__builtin_unreachable();
}

static inline bool i2c_initiate_transfer(uint32_t i2c_base, uint8_t device_register, uint8_t reg)
{
	return try_get_device(i2c_base, device_register) && try_get_register(i2c_base, reg);
}

bool i2c_write(uint8_t device_id, uint8_t reg, uint8_t data)
{
	uint8_t bus_id = i2c_get_bus_id(device_id);
	uint8_t device_register = i2c_get_device_register(device_id);
	uint32_t i2c_base = i2c_bus_id_to_addr(bus_id);
	if (!i2c_base)
		return false;

	for (uint8_t i = 0; i < 8; i++)
	{
		if (i2c_initiate_transfer(i2c_base, device_register, reg))
		{
			i2c_wait(i2c_base);
			i2c_write_data_reg(i2c_base, data);
			i2c_write_cnt_reg(i2c_base, CNT_STOP | CNT_INT_ENABLE | CNT_CHECK_AVAILABILITY);

			if (i2c_was_acknowledged(i2c_base))
				return true;
		}
		i2c_nack(i2c_base);
	}

	return false;
}
