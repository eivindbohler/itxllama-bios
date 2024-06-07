#ifndef __LLAMA_NBSB_H
#define __LLAMA_NBSB_H

#include "types.h"

typedef enum {
  vx86ex_nb = 0,
  vx86ex_sb = 7
} vx86ex_nbsb;

typedef enum {
  vx86ex_nbsb_func0,
  vx86ex_nbsb_func1
} vx86ex_nbsb_func;

u32 llama_nbsb_read32(vx86ex_nbsb nbsb, vx86ex_nbsb_func func, u8 offset);
void llama_nbsb_write32(vx86ex_nbsb nbsb, vx86ex_nbsb_func func, u8 offset, u32 value);
u8 llama_nbsb_read8(vx86ex_nbsb nbsb, vx86ex_nbsb_func func, u8 offset);
void llama_nbsb_write8(vx86ex_nbsb nbsb, vx86ex_nbsb_func func, u8 offset, u8 value);

#endif // __LLAMA_NBSB_H