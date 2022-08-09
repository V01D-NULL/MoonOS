#ifndef I2C_H
#define I2C_H

#include <base/base-types.h>

bool i2c_write(uint8_t device_id, uint8_t reg, uint8_t data);

#endif // I2C_H