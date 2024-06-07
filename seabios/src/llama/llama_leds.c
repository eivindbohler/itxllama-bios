#include "llama/llama_i2c.h"
#include "types.h"
#include "util.h" // mdelay

#define LEDS_ADDR 0x20

#define LEDS_LED_ENABLED_REG     0x01  // RW - 0x00 - disabled, 0x01 - enabled
#define LEDS_LED_INTENSITY_REG   0x02  // RW - 0x00-0xFF, 0-100% intensity

#define LEDS_CLICK_ENABLED_REG   0x11  // RW - 0x00 - disabled, 0x01 - enabled

static u8 leds_get_reg_byte(u8 reg) {
  u8 reg_value;
  if (i2c_start(LEDS_ADDR, 0) && i2c_write(&reg, 1)) {
    mdelay(5);
    if (i2c_start(LEDS_ADDR, 1) && i2c_read(&reg_value, 1)) {
      return reg_value;
    }
  }
  return 0;
}

static int leds_set_reg_byte(u8 reg, u8 reg_value) {
  if (i2c_start(LEDS_ADDR, 0) && i2c_write(&reg, 1)) {
    mdelay(5);
    return i2c_start(LEDS_ADDR, 0) && i2c_write(&reg_value, 1);
  }
  return 0;
}

void llama_set_leds_enabled(u8 leds_enabled) {
  leds_set_reg_byte(LEDS_LED_ENABLED_REG, leds_enabled);
}

u8 llama_get_leds_enabled(void) {
  return leds_get_reg_byte(LEDS_LED_ENABLED_REG);
}

void llama_set_leds_intensity(u8 leds_intensity) {
  leds_set_reg_byte(LEDS_LED_INTENSITY_REG, leds_intensity);
}

u8 llama_get_leds_intensity(void) {
  return leds_get_reg_byte(LEDS_LED_INTENSITY_REG);
}

void llama_set_click_enabled(u8 click_enabled) {
  leds_set_reg_byte(LEDS_CLICK_ENABLED_REG, click_enabled);
}

u8 llama_get_click_enabled(void) {
  return leds_get_reg_byte(LEDS_CLICK_ENABLED_REG);
}