#include "x86.h"
#include "output.h"
#include "util.h"
#include "string.h"
#include "llama/llama_nbsb.h"

#define SPI_ROM_SIZE     8L * 1024 * 1024
#define SPI_SECTOR_SIZE  4096
#define SPI_PAGE_SIZE    256

const u32 spi_sector_size          = SPI_SECTOR_SIZE;
const u32 spi_page_size            = SPI_PAGE_SIZE;
const u32 spi_sector_offset        = SPI_ROM_SIZE - SPI_SECTOR_SIZE;
const u32 spi_crossbar_offset      = SPI_ROM_SIZE - (3 * SPI_PAGE_SIZE);
const u32 spi_bios_settings_offset = SPI_ROM_SIZE - SPI_PAGE_SIZE;

static void write_spi_byte(u16 iobase, u8 n)
{
  outb(n, iobase);
  while((inb(iobase + 3) & 0x10) == 0);
}

static u8 read_spi_byte(u16 iobase)
{
  outb(0, iobase + 1); // trigger SPI read
  while((inb(iobase + 3) & 0x20) == 0);
  return inb(iobase + 1); // read SPI data
}

static void enable_cs(u16 iobase)
{
  outb(0, iobase + 4);
}

static void disable_cs(u16 iobase)
{
  outb(1, iobase + 4);
}

static void write_spi_24bit_addr(u16 iobase, u32 addr)
{
  write_spi_byte(iobase, (u8)((addr >> 16) & 0xFF));
  write_spi_byte(iobase, (u8)((addr >> 8) & 0xFF));
  write_spi_byte(iobase, (u8)((addr) & 0xFF));
}

static u8 reg_sb_c4    = 0;
static u32 reg_nb_40   = 0L;
static u8 spi_ctrl_reg = 0;
static u32 spi_base    = 0L;

typedef struct flash_info {
  u8 id[3];
  const char *name;
  u32 size;
} flash_info;

static const flash_info flash_info_table[] = {
  {{0xc2, 0x20, 0x12}, "MX25L2005", 256L * 1024},
  {{0xc2, 0x20, 0x15}, "MX25L1605", 2L * 1024 * 1024},
  {{0xc2, 0x25, 0x37}, "MX25U6435F", 8L * 1024 * 1024},
  {{0xc2, 0x25, 0x38}, "MX25U12835F", 16L * 1024 * 1024}
};

static const int flash_info_table_size = sizeof(flash_info_table) / sizeof(flash_info_table[0]);

static const flash_info *get_flash_info(u8 id[3])
{
  int i;
  for (i = 0; i < flash_info_table_size; i++) {
    if (memcmp(id, flash_info_table[i].id, 3) == 0) {
      return flash_info_table + i;
    }
  }
  return NULL;
}

static void read_flash_device_id(u8 *p)
{
  enable_cs(spi_base);
  write_spi_byte(spi_base, 0x9F); // RDID command

  //read device ID
  p[0] = read_spi_byte(spi_base);
  p[1] = read_spi_byte(spi_base);
  p[2] = read_spi_byte(spi_base);
  disable_cs(spi_base);
}

static void set_flash_writable(void)
{
  reg_sb_c4 = llama_nbsb_read8(vx86ex_sb, vx86ex_nbsb_func0, 0xC4);
  reg_nb_40 = llama_nbsb_read32(vx86ex_nb, vx86ex_nbsb_func0, 0x40);
  spi_base = reg_nb_40 & 0x00FFF0;
  spi_ctrl_reg = inb(spi_base + 2);

  llama_nbsb_write8(vx86ex_sb, vx86ex_nbsb_func0, 0xC4, reg_sb_c4 | 1);
  llama_nbsb_write32(vx86ex_nb, vx86ex_nbsb_func0, 0x40, reg_nb_40 | 1);

  outl(((spi_ctrl_reg | 0x20) & 0xF0) | 0x0C, spi_base + 2);
}

static void set_flash_unwritable(void)
{
  outl(spi_ctrl_reg, spi_base + 2);
  llama_nbsb_write32(vx86ex_nb, vx86ex_nbsb_func0, 0x40, reg_nb_40);
  llama_nbsb_write8(vx86ex_sb, vx86ex_nbsb_func0, 0xC4, reg_sb_c4);
}

void spi_flash_write_byte(u32 in_addr, u8 in_value)
{
  u8 s;
  int wip_cnt = 0;

  set_flash_writable();

  //mxic write enable
  enable_cs(spi_base);
  write_spi_byte(spi_base, 0x06);
  disable_cs(spi_base);

  //write data
  enable_cs(spi_base);
  write_spi_byte(spi_base, 0x02); //PAGE PROGRAM
  write_spi_24bit_addr(spi_base, in_addr); //address
  write_spi_byte(spi_base, in_value);
  disable_cs(spi_base);

  //wait wip
  enable_cs(spi_base);
  write_spi_byte(spi_base, 0x05); //RDSR
  while (1) {
    s = read_spi_byte(spi_base);
    if ((s & 1) == 0) { //WIP == 0
      wip_cnt++;
      if (wip_cnt >= 3) break;
    } else {
      wip_cnt = 0;
    }
  }
  disable_cs(spi_base);

  //mxic write disable
  enable_cs(spi_base);
  write_spi_byte(spi_base, 0x04);
  disable_cs(spi_base);

  set_flash_unwritable();
}

void spi_flash_write_word(u32 in_addr, u16 in_value)
{
  spi_flash_write_byte(in_addr, (u8)(in_value >> 8));
  spi_flash_write_byte(in_addr + 1, (u8)(in_value & 0xFF));
}

void spi_flash_erase_sector(u32 in_addr)
{
  u8 s;
  int wip_cnt = 0;

  set_flash_writable();

  //mxic write enable
  enable_cs(spi_base);
  write_spi_byte(spi_base, 0x06);
  disable_cs(spi_base);

  //reset command and address
  enable_cs(spi_base);
  write_spi_byte(spi_base, 0x20);
  write_spi_24bit_addr(spi_base, in_addr);
  disable_cs(spi_base);

  //wait wip
  enable_cs(spi_base);
  write_spi_byte(spi_base, 0x05); //RDSR
  while(1) {
    s = read_spi_byte(spi_base);
    if ((s & 1) == 0) { //WIP == 0
      wip_cnt ++;
      if (wip_cnt >= 3) break;
    } else {
      wip_cnt = 0;
    }
  }
  disable_cs(spi_base);

  //mxic write disable
  enable_cs(spi_base);
  write_spi_byte(spi_base, 0x04);
  disable_cs(spi_base);

  set_flash_unwritable();
}

u8 spi_flash_read_byte(u32 in_addr)
{
  u8 value;
  set_flash_writable();
  enable_cs(spi_base);
  write_spi_byte(spi_base, 0x03); //READ
  write_spi_24bit_addr(spi_base, in_addr); //address
  value = read_spi_byte(spi_base);
  disable_cs(spi_base);
  set_flash_unwritable();
  return value;
}

u16 spi_flash_read_word(u32 in_addr)
{
  return (spi_flash_read_byte(in_addr) | spi_flash_read_byte(in_addr + 1) << 8);
}

int get_spi_flash_info(void)
{
  set_flash_writable();
  u8 id[3];
  read_flash_device_id(id);
  set_flash_unwritable();

  const flash_info *pfi = get_flash_info(id);
  if (pfi != NULL) {
    dprintf(1, "Found SPI flash chip: %s, size: %d\n", pfi->name, pfi->size);
    return 1;
  } else {
    dprintf(1, "Could not find a SPI flash chip.\n");
    return 0;
  }
}