#ifndef __LLAMA_BIOS_SETTINGS_H
#define __LLAMA_BIOS_SETTINGS_H

#include "types.h"

typedef struct {
  u8 value;
  u8 offset;
} llama_bios_setting_entry_u8;

typedef struct {
  u16 value;
  u8 offset;
} llama_bios_setting_entry_u16;

struct llama_bios_settings_s {
  u8 has_changes; // 0 = no, 1 = yes
  u8 cs4237_ext_fm_present;

  // 0 = no, 1 = yes (means the settings have been saved to SPI flash)
  llama_bios_setting_entry_u8 initialized;

  llama_bios_setting_entry_u8 cpu_freq_index;
  llama_bios_setting_entry_u8 l1_cache;              // 0 = no, 1 = yes
  llama_bios_setting_entry_u8 l2_cache;              // 0 = no, 1 = yes
  llama_bios_setting_entry_u8 boot_order_index;
  llama_bios_setting_entry_u8 usb_as_fixed_disks;    // 0 = no, 1 = yes
  llama_bios_setting_entry_u8 com1_base_clock_index; // 0 = 115200, 1 = 3M, 2 = 6M
  llama_bios_setting_entry_u8 com2_base_clock_index; // 0 = 115200, 1 = 3M, 2 = 6M
  llama_bios_setting_entry_u16 cs4237_wss_port;      // default: 0x534
  llama_bios_setting_entry_u16 cs4237_syn_port;      // default: 0x388
  llama_bios_setting_entry_u16 cs4237_sb_port;       // default: 0x220
  llama_bios_setting_entry_u16 cs4237_gamepad_port;  // default: 0x200
  llama_bios_setting_entry_u16 cs4237_mpu_port;      // default: 0x330
  llama_bios_setting_entry_u8 cs4237_ext_fm;         // 0 = no, 1 = yes
  llama_bios_setting_entry_u8 cs4237_digital;        // 0 = no, 1 = yes
  llama_bios_setting_entry_u8 mac_address_index;
  llama_bios_setting_entry_u16 fan1_target_rpm;
  llama_bios_setting_entry_u16 fan2_target_rpm;
  llama_bios_setting_entry_u16 fan3_target_rpm;
  llama_bios_setting_entry_u8 boot_tune_index;       // 0 = none, 1 = ducks, 2 = mushroom
} llama_bios_settings_default = {
  .has_changes           = 0,
  .cs4237_ext_fm_present = 0,
  .initialized           = { 1,     0xC0 },
  .cpu_freq_index        = { 3,     0xC1 },
  .l1_cache              = { 1,     0xC2 },
  .l2_cache              = { 1,     0xC3 },
  .boot_order_index      = { 0,     0xC4 },
  .usb_as_fixed_disks    = { 1,     0xC5 },
  .com1_base_clock_index = { 0,     0xC6 },
  .com2_base_clock_index = { 0,     0xC7 },
  .cs4237_wss_port       = { 0x534, 0xC8 },
  .cs4237_syn_port       = { 0x388, 0xCA },
  .cs4237_sb_port        = { 0x220, 0xCC },
  .cs4237_gamepad_port   = { 0x200, 0xCE },
  .cs4237_mpu_port       = { 0x330, 0xD0 },
  .cs4237_ext_fm         = { 0,     0xD2 },
  .cs4237_digital        = { 1,     0xD3 },
  .mac_address_index     = { 0,     0xD4 },
  .fan1_target_rpm       = { 2000,  0xD5 },
  .fan2_target_rpm       = { 2000,  0xD7 },
  .fan3_target_rpm       = { 2000,  0xD9 },
  .boot_tune_index       = { 1,     0xDB }
};
typedef struct llama_bios_settings_s llama_bios_settings;
extern llama_bios_settings llama_bios_settings_default;

void llama_load_bios_settings(llama_bios_settings *lbs);
void llama_save_bios_settings(llama_bios_settings *lbs);

#endif // __LLAMA_BIOS_SETTINGS_H