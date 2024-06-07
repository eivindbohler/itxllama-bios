#include "llama/llama_bios_menu_handlers.h"
#include "llama/llama_emc2303.h"
#include "llama/llama_leds.h"
#include "string.h"

u16 handler_get_cpu_freq_index(llama_bios_settings *lbs) {
  return lbs->cpu_freq_index.value;
}

u16 handler_get_l1_cache(llama_bios_settings *lbs) {
  return lbs->l1_cache.value;
}

u16 handler_get_l2_cache(llama_bios_settings *lbs) {
  return lbs->l2_cache.value;
}

u16 handler_get_boot_order_index(llama_bios_settings *lbs) {
  return lbs->boot_order_index.value;
}

u16 handler_get_usb_as_fixed_disks(llama_bios_settings *lbs) {
  return lbs->usb_as_fixed_disks.value;
}

u16 handler_get_com1_base_clock_index(llama_bios_settings *lbs) {
  return lbs->com1_base_clock_index.value;
}

u16 handler_get_com2_base_clock_index(llama_bios_settings *lbs) {
  return lbs->com2_base_clock_index.value;
}

u16 handler_get_cs4237_wss_port(llama_bios_settings *lbs) {
  return lbs->cs4237_wss_port.value;
}

u16 handler_get_cs4237_syn_port(llama_bios_settings *lbs) {
  return lbs->cs4237_syn_port.value;
}

u16 handler_get_cs4237_sb_port(llama_bios_settings *lbs) {
  return lbs->cs4237_sb_port.value;
}

u16 handler_get_cs4237_gamepad_port(llama_bios_settings *lbs) {
  return lbs->cs4237_gamepad_port.value;
}

u16 handler_get_cs4237_mpu_port(llama_bios_settings *lbs) {
  return lbs->cs4237_mpu_port.value;
}

u16 handler_get_cs4237_ext_fm(llama_bios_settings *lbs) {
  return lbs->cs4237_ext_fm.value;
}

u16 handler_get_cs4237_digital(llama_bios_settings *lbs) {
  return lbs->cs4237_digital.value;
}

u16 handler_get_mac_address_index(llama_bios_settings *lbs) {
  return lbs->mac_address_index.value;
}

u16 handler_get_fan1_target_rpm(llama_bios_settings *lbs) {
  return lbs->fan1_target_rpm.value;
}

u16 handler_get_fan2_target_rpm(llama_bios_settings *lbs) {
  return lbs->fan2_target_rpm.value;
}

u16 handler_get_fan3_target_rpm(llama_bios_settings *lbs) {
  return lbs->fan3_target_rpm.value;
}

u16 handler_get_boot_tune_index(llama_bios_settings *lbs) {
  return lbs->boot_tune_index.value;
}

u16 handler_get_leds_enabled(llama_bios_settings *lbs) {
  return llama_get_leds_enabled();
}

u16 handler_get_leds_intensity(llama_bios_settings *lbs) {
  return llama_get_leds_intensity();
}

u16 handler_get_click_enabled(llama_bios_settings *lbs) {
  return llama_get_click_enabled();
}

void handler_set_cpu_freq_index(llama_bios_settings *lbs, u16 value)
{
  u8 val = (value & 0xFF);
  if (val != lbs->cpu_freq_index.value) {
    lbs->cpu_freq_index.value = val;
    lbs->has_changes = 1;
  }
}

void handler_set_l1_cache(llama_bios_settings *lbs, u16 value)
{
  u8 val = (value & 0xFF);
  if (val != lbs->l1_cache.value) {
    lbs->l1_cache.value = val;
    lbs->has_changes = 1;
  }
}

void handler_set_l2_cache(llama_bios_settings *lbs, u16 value)
{
  u8 val = (value & 0xFF);
  if (val != lbs->l2_cache.value) {
    lbs->l2_cache.value = val;
    lbs->has_changes = 1;
  }
}

void handler_set_boot_order_index(llama_bios_settings *lbs, u16 value)
{
  u8 val = (value & 0xFF);
  if (val != lbs->boot_order_index.value) {
    lbs->boot_order_index.value = val;
    lbs->has_changes = 1;
  }
}

void handler_set_usb_as_fixed_disks(llama_bios_settings *lbs, u16 value)
{
  u8 val = (value & 0xFF);
  if (val != lbs->usb_as_fixed_disks.value) {
    lbs->usb_as_fixed_disks.value = val;
    lbs->has_changes = 1;
  }
}

void handler_set_com1_base_clock_index(llama_bios_settings *lbs, u16 value)
{
  u8 val = (value & 0xFF);
  if (val != lbs->com1_base_clock_index.value) {
    lbs->com1_base_clock_index.value = val;
    lbs->has_changes = 1;
  }
}

void handler_set_com2_base_clock_index(llama_bios_settings *lbs, u16 value)
{
  u8 val = (value & 0xFF);
  if (val != lbs->com2_base_clock_index.value) {
    lbs->com2_base_clock_index.value = val;
    lbs->has_changes = 1;
  }
}

void handler_set_cs4237_wss_port(llama_bios_settings *lbs, u16 value)
{
  if (value != lbs->cs4237_wss_port.value) {
    lbs->cs4237_wss_port.value = value;
    lbs->has_changes = 1;
  }
}

void handler_set_cs4237_syn_port(llama_bios_settings *lbs, u16 value)
{
  if (value != lbs->cs4237_syn_port.value) {
    lbs->cs4237_syn_port.value = value;
    lbs->has_changes = 1;
  }
}

void handler_set_cs4237_sb_port(llama_bios_settings *lbs, u16 value)
{
  if (value != lbs->cs4237_sb_port.value) {
    lbs->cs4237_sb_port.value = value;
    lbs->has_changes = 1;
  }
}

void handler_set_cs4237_gamepad_port(llama_bios_settings *lbs, u16 value)
{
  if (value != lbs->cs4237_gamepad_port.value) {
    lbs->cs4237_gamepad_port.value = value;
    lbs->has_changes = 1;
  }
}

void handler_set_cs4237_mpu_port(llama_bios_settings *lbs, u16 value)
{
  if (value != lbs->cs4237_mpu_port.value) {
    lbs->cs4237_mpu_port.value = value;
    lbs->has_changes = 1;
  }
}

void handler_set_cs4237_ext_fm(llama_bios_settings *lbs, u16 value)
{
  u8 val = (value & 0xFF);
  if (val != lbs->cs4237_ext_fm.value) {
    lbs->cs4237_ext_fm.value = val;
    lbs->has_changes = 1;
  }
}

void handler_set_cs4237_digital(llama_bios_settings *lbs, u16 value)
{
  u8 val = (value & 0xFF);
  if (val != lbs->cs4237_digital.value) {
    lbs->cs4237_digital.value = val;
    lbs->has_changes = 1;
  }
}

void handler_set_mac_address_index(llama_bios_settings *lbs, u16 value)
{
  u8 val = (value & 0xFF);
  if (val != lbs->mac_address_index.value) {
    lbs->mac_address_index.value = val;
    lbs->has_changes = 1;
  }
}

void handler_set_fan1_target_rpm(llama_bios_settings *lbs, u16 value)
{
  if (value != lbs->fan1_target_rpm.value) {
    lbs->fan1_target_rpm.value = value;
    lbs->has_changes = 1;
    llama_set_fan1_target_rpm(value);
  }
}

void handler_set_fan2_target_rpm(llama_bios_settings *lbs, u16 value)
{
  if (value != lbs->fan2_target_rpm.value) {
    lbs->fan2_target_rpm.value = value;
    lbs->has_changes = 1;
    llama_set_fan2_target_rpm(value);
  }
}

void handler_set_fan3_target_rpm(llama_bios_settings *lbs, u16 value)
{
  if (value != lbs->fan3_target_rpm.value) {
    lbs->fan3_target_rpm.value = value;
    lbs->has_changes = 1;
    llama_set_fan3_target_rpm(value);
  }
}

void handler_set_boot_tune_index(llama_bios_settings *lbs, u16 value)
{
  u8 val = (value & 0xFF);
  if (val != lbs->boot_tune_index.value) {
    lbs->boot_tune_index.value = val;
    lbs->has_changes = 1;
  }
}

void handler_set_leds_enabled(llama_bios_settings *lbs, u16 value)
{
  u8 val = (value & 0xFF);
  llama_set_leds_enabled(val);
}

void handler_set_leds_intensity(llama_bios_settings *lbs, u16 value)
{
  u8 val = (value & 0xFF);
  llama_set_leds_intensity(val);
}

void handler_set_click_enabled(llama_bios_settings *lbs, u16 value)
{
  u8 val = (value & 0xFF);
  llama_set_click_enabled(val);
}