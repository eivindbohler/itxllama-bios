#ifndef __LLAMA_I2C_H
#define __LLAMA_I2C_H

#include "types.h"

int i2c_start(u8 addr, u8 rwbit);
int i2c_read(u8 *data, int len);
int i2c_write(u8 *data, int len);
int i2c_write_byte_no_stop(u8 data);

#endif // __LLAMA_I2C_H