#ifndef __LLAMA_SPI_FLASH_H
#define __LLAMA_SPI_FLASH_H

#include "types.h"

extern const u32 spi_page_size;
extern const u32 spi_sector_offset;
extern const u32 spi_crossbar_offset;
extern const u32 spi_bios_settings_offset;
int get_spi_flash_info(void);
u8 spi_flash_read_byte(u32);
u16 spi_flash_read_word(u32);
void spi_flash_write_byte(u32, u8);
void spi_flash_write_word(u32, u16);
void spi_flash_erase_sector(u32);

#endif // __LLAMA_SPI_FLASH_H