#include "types.h"   // u32
#include "util.h"    // timer_calc, timer_check, warn_timeout, yield
#include "x86.h"     // inb
#include "stacks.h"  // yield
//#include "output.h"  // printf

#define I2C_BASE            0xfb00
#define I2C_CTRL_REG        I2C_BASE
#define I2C_STAT_REG        I2C_BASE + 0x01
#define I2C_SLAVE_ADDR_REG  I2C_BASE + 0x02
#define I2C_ADDR_REG        I2C_BASE + 0x03
#define I2C_DATA_REG        I2C_BASE + 0x04

#define I2C_TIMEOUT         2000 // ms

static inline int i2c_check_stop(void)
{
  return inb(I2C_CTRL_REG) & 0x02;
}

static inline void i2c_set_stop(void)
{
  outb((inb(I2C_CTRL_REG) | 0x02), I2C_CTRL_REG);
}

static inline u8 i2c_read_stat_reg(void)
{
  return inb(I2C_STAT_REG);
}

static inline void i2c_clear_ar_loss(void)
{
  outb(0x08, I2C_STAT_REG);
}

static inline void i2c_clear_ack_error(void)
{
  outb(0x10, I2C_STAT_REG);
}

static inline int i2c_check_tx_done(void)
{
  return inb(I2C_STAT_REG) & 0x20;
}

static inline void i2c_clear_tx_done(void)
{
  outb(0x20, I2C_STAT_REG);
}

static inline int i2c_check_rx_ready(void)
{
  return inb(I2C_STAT_REG) & 0x40;
}

static inline void i2c_clear_rx_ready(void)
{
  outb(0x40, I2C_STAT_REG);
}

static inline void i2c_set_address(u8 addr, u8 rw_bit)
{
  outb(((addr << 1) + rw_bit), I2C_ADDR_REG);
}

static inline u8 i2c_read_byte(void)
{
  return inb(I2C_DATA_REG);
}

static inline void i2c_write_byte(u8 data)
{
  outb(data, I2C_DATA_REG);
}

static int check_tx_done(void)
{
  u8 stat_reg;
  u32 end = timer_calc(I2C_TIMEOUT);
  for (;;) {
    if (i2c_check_tx_done()) break;
    if (timer_check(end)) {
      //printf("I2C: check_tx_done timeout\n");
      break;
    }
    yield();
  }
  stat_reg = i2c_read_stat_reg();
  i2c_clear_tx_done();

  if ((stat_reg & 0x08) != 0) {
    //printf("I2C: arbitration loss\n");
    i2c_clear_ar_loss();
  }
  if ((stat_reg & 0x10) != 0) {
    //printf("I2C: did not receive ACK after transmitting\n");
    i2c_clear_ack_error();
  }
  if ((stat_reg & 0x18) != 0) return 0;

  if ((stat_reg & 0x01) == 0) {
    //printf("I2C: not in master mode\n");
    return 0;
  }

  if ((stat_reg & 0x20) == 0) {
    //printf("I2C: module not responding\n");
    return 0;
  }

  return 1;
}

static int check_rx_done(void)
{
  u8 stat_reg;
  u32 end = timer_calc(I2C_TIMEOUT);
  for (;;) {
    if (i2c_check_rx_ready()) break;
    if (timer_check(end)) {
      //printf("I2C: check_rx_done timeout\n");
      break;
    }
    yield();
  }
  stat_reg = i2c_read_stat_reg();
  i2c_clear_rx_ready();

  if ((stat_reg & 0x08) != 0) {
    //printf("I2C: arbitration loss\n");
    i2c_clear_ar_loss();
    return 0;
  }

  if ((stat_reg & 0x01) == 0) {
    //printf("I2C: not in master mode\n");
    return 0;
  }

  if ((stat_reg & 0x40) == 0) {
    //printf("I2C: module not responding\n");
    return 0;
  }

  return 1;
}

static int check_stop_done(void)
{
  u32 end = timer_calc(I2C_TIMEOUT);
  for (;;) {
    if (i2c_check_stop() == 0) break;
    if (timer_check(end)) {
      //printf("I2C: check_stop_done timeout\n");
      break;
    }
    yield();
  }

  if (i2c_check_stop()) {
    //printf("I2C: failed to stop transaction\n");
    return 0;
  }

  return 1;
}

int i2c_start(u8 addr, u8 rwbit)
{
  i2c_set_address(addr, rwbit);
  if (!check_tx_done()) {
    //printf("i2c_start: i2c_set_address check_tx_done failed\n");
    return 0;
  }
  return 1;
}

int i2c_read(u8 *data, int len)
{
  int i;
  for (i = 0; i < len; i++) {
    if (i == 0) {
      if (i == len - 1) {
        i2c_set_stop();
      }
      i2c_read_byte(); // dummy read
    }
    if (!check_rx_done()) {
      //printf("i2c_read: check_rx_done failed\n");
      return 0;
    }
    if (i == len - 2) {
      i2c_set_stop();
    }
    data[i] = i2c_read_byte();
    if (i == len - 1) {
      if (!check_stop_done()) {
        //printf("i2c_read: check_stop_done failed\n");
        return 0;
      }
    }
  }
  return 1;
}

int i2c_write(u8 *data, int len)
{
  int i;
  for (i = 0; i < len; i++) {
    i2c_write_byte(data[i]);
    if (!check_tx_done()) {
      //printf("i2c_write: i2c_write_byte check_tx_done failed\n");
      return 0;
    }
  }
  i2c_set_stop();
  if (!check_stop_done()) {
    //printf("i2c_write: check_stop_done failed\n");
    return 0;
  }
  return 1;
}

int i2c_write_byte_no_stop(u8 data)
{
  i2c_write_byte(data);
  if (!check_tx_done()) {
    //printf("i2c_write_byte_no_stop: check_tx_done failed\n");
    return 0;
  }
  return 1;
}