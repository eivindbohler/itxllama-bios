#include "llama/llama_bios_settings.h"
#include "llama/llama_spi_flash.h"
#include "types.h"

static const u8 freq_array[6][6] = {
  {0x48, 0x37, 0x21, 0x02, 0xCF, 0x07},  //  60/150/60
  {0x40, 0x26, 0x23, 0x02, 0x3F, 0x07},  // 100/200/33
  {0x30, 0x03, 0x23, 0x02, 0xDF, 0x07},  // 200/200/66
  {0x48, 0x03, 0x23, 0x02, 0x7F, 0x07},  // 300/300/100
  {0xA8, 0x53, 0x23, 0x02, 0x3F, 0x07},  // 466/350/155
  {0x78, 0x52, 0x23, 0x02, 0xDF, 0x07},  // 500/375/166
};

static const u8 mac_address_array[6][6] = {
  {0x00, 0x0B, 0xB4, 0x00, 0x00, 0x01},
  {0x00, 0x0B, 0xB4, 0x00, 0x00, 0x02},
  {0x00, 0x0B, 0xB4, 0x00, 0x00, 0x03},
  {0x00, 0x0B, 0xB4, 0x00, 0x00, 0x04},
  {0x00, 0x0B, 0xB4, 0x00, 0x00, 0x05},
  {0x00, 0x0B, 0xB4, 0x00, 0x00, 0x06}
};

void llama_load_bios_settings(llama_bios_settings *lbs)
{
  u8 initialized = spi_flash_read_byte(spi_bios_settings_offset + lbs->initialized.offset);
  if (initialized != 1) return;

  lbs->cpu_freq_index.value = spi_flash_read_byte(
    spi_bios_settings_offset + lbs->cpu_freq_index.offset
  );
  lbs->l1_cache.value = spi_flash_read_byte(
    spi_bios_settings_offset + lbs->l1_cache.offset
  );
  lbs->l2_cache.value = spi_flash_read_byte(
    spi_bios_settings_offset + lbs->l2_cache.offset
  );
  lbs->boot_order_index.value = spi_flash_read_byte(
    spi_bios_settings_offset + lbs->boot_order_index.offset
  );
  lbs->usb_as_fixed_disks.value = spi_flash_read_byte(
    spi_bios_settings_offset + lbs->usb_as_fixed_disks.offset
  );
  lbs->com1_base_clock_index.value = spi_flash_read_byte(
    spi_bios_settings_offset + lbs->com1_base_clock_index.offset
  );
  lbs->com2_base_clock_index.value = spi_flash_read_byte(
    spi_bios_settings_offset + lbs->com2_base_clock_index.offset
  );
  lbs->cs4237_wss_port.value = spi_flash_read_word(
    spi_bios_settings_offset + lbs->cs4237_wss_port.offset
  );
  lbs->cs4237_syn_port.value = spi_flash_read_word(
    spi_bios_settings_offset + lbs->cs4237_syn_port.offset
  );
  lbs->cs4237_sb_port.value = spi_flash_read_word(
    spi_bios_settings_offset + lbs->cs4237_sb_port.offset
  );
  lbs->cs4237_gamepad_port.value = spi_flash_read_word(
    spi_bios_settings_offset + lbs->cs4237_gamepad_port.offset
  );
  lbs->cs4237_mpu_port.value = spi_flash_read_word(
    spi_bios_settings_offset + lbs->cs4237_mpu_port.offset
  );
  lbs->cs4237_ext_fm.value = spi_flash_read_byte(
    spi_bios_settings_offset + lbs->cs4237_ext_fm.offset
  );
  lbs->cs4237_digital.value = spi_flash_read_byte(
    spi_bios_settings_offset + lbs->cs4237_digital.offset
  );
  lbs->mac_address_index.value = spi_flash_read_byte(
    spi_bios_settings_offset + lbs->mac_address_index.offset
  );
  lbs->fan1_target_rpm.value = spi_flash_read_word(
    spi_bios_settings_offset + lbs->fan1_target_rpm.offset
  );
  lbs->fan2_target_rpm.value = spi_flash_read_word(
    spi_bios_settings_offset + lbs->fan2_target_rpm.offset
  );
  lbs->fan3_target_rpm.value = spi_flash_read_word(
    spi_bios_settings_offset + lbs->fan3_target_rpm.offset
  );
  lbs->boot_tune_index.value = spi_flash_read_byte(
    spi_bios_settings_offset + lbs->boot_tune_index.offset
  );
}

void llama_save_bios_settings(llama_bios_settings *lbs)
{
  int i;
  u8 crossbar[spi_page_size];
  u8 bios_settings[spi_page_size];

  if (get_spi_flash_info() == 0) {
    return;
  }
  for (i = 0; i < spi_page_size; i++) {
    crossbar[i] = spi_flash_read_byte(spi_crossbar_offset + i);
  }

  for (i = 0; i < spi_page_size; i++) {
    bios_settings[i] = spi_flash_read_byte(spi_bios_settings_offset + i);
  }

  bios_settings[0xB0] = mac_address_array[lbs->mac_address_index.value][0];
  bios_settings[0xB1] = mac_address_array[lbs->mac_address_index.value][1];
  bios_settings[0xB2] = mac_address_array[lbs->mac_address_index.value][2];
  bios_settings[0xB3] = mac_address_array[lbs->mac_address_index.value][3];
  bios_settings[0xB4] = mac_address_array[lbs->mac_address_index.value][4];
  bios_settings[0xB5] = mac_address_array[lbs->mac_address_index.value][5];

  bios_settings[0xB6] = freq_array[lbs->cpu_freq_index.value][0];
  bios_settings[0xB7] = freq_array[lbs->cpu_freq_index.value][1];
  bios_settings[0xBB] = freq_array[lbs->cpu_freq_index.value][2];
  bios_settings[0xBC] = freq_array[lbs->cpu_freq_index.value][3];
  bios_settings[0xBD] = freq_array[lbs->cpu_freq_index.value][4];
  bios_settings[0xBF] = freq_array[lbs->cpu_freq_index.value][5];

  bios_settings[lbs->initialized.offset] = 1;
  bios_settings[lbs->cpu_freq_index.offset] = lbs->cpu_freq_index.value;
  bios_settings[lbs->l1_cache.offset] = lbs->l1_cache.value;
  bios_settings[lbs->l2_cache.offset] = lbs->l2_cache.value;
  bios_settings[lbs->boot_order_index.offset] = lbs->boot_order_index.value;
  bios_settings[lbs->usb_as_fixed_disks.offset] = lbs->usb_as_fixed_disks.value;
  bios_settings[lbs->com1_base_clock_index.offset] = lbs->com1_base_clock_index.value;
  bios_settings[lbs->com2_base_clock_index.offset] = lbs->com2_base_clock_index.value;
  bios_settings[lbs->cs4237_wss_port.offset] = (u8)(lbs->cs4237_wss_port.value & 0xFF);
  bios_settings[lbs->cs4237_wss_port.offset + 1] = (u8)((lbs->cs4237_wss_port.value >> 8) & 0xFF);
  bios_settings[lbs->cs4237_syn_port.offset] = (u8)(lbs->cs4237_syn_port.value & 0xFF);
  bios_settings[lbs->cs4237_syn_port.offset + 1] = (u8)((lbs->cs4237_syn_port.value >> 8) & 0xFF);
  bios_settings[lbs->cs4237_sb_port.offset] = (u8)(lbs->cs4237_sb_port.value & 0xFF);
  bios_settings[lbs->cs4237_sb_port.offset + 1] = (u8)((lbs->cs4237_sb_port.value >> 8) & 0xFF);
  bios_settings[lbs->cs4237_gamepad_port.offset] = (u8)(lbs->cs4237_gamepad_port.value & 0xFF);
  bios_settings[lbs->cs4237_gamepad_port.offset + 1] = (u8)((lbs->cs4237_gamepad_port.value >> 8) & 0xFF);
  bios_settings[lbs->cs4237_mpu_port.offset] = (u8)(lbs->cs4237_mpu_port.value & 0xFF);
  bios_settings[lbs->cs4237_mpu_port.offset + 1] = (u8)((lbs->cs4237_mpu_port.value >> 8) & 0xFF);
  bios_settings[lbs->cs4237_ext_fm.offset] = lbs->cs4237_ext_fm.value;
  bios_settings[lbs->cs4237_digital.offset] = lbs->cs4237_digital.value;
  bios_settings[lbs->mac_address_index.offset] = lbs->mac_address_index.value;
  bios_settings[lbs->fan1_target_rpm.offset] = (u8)(lbs->fan1_target_rpm.value & 0xFF);
  bios_settings[lbs->fan1_target_rpm.offset + 1] = (u8)((lbs->fan1_target_rpm.value >> 8) & 0xFF);
  bios_settings[lbs->fan2_target_rpm.offset] = (u8)(lbs->fan2_target_rpm.value & 0xFF);
  bios_settings[lbs->fan2_target_rpm.offset + 1] = (u8)((lbs->fan2_target_rpm.value >> 8) & 0xFF);
  bios_settings[lbs->fan3_target_rpm.offset] = (u8)(lbs->fan3_target_rpm.value & 0xFF);
  bios_settings[lbs->fan3_target_rpm.offset + 1] = (u8)((lbs->fan3_target_rpm.value >> 8) & 0xFF);
  bios_settings[lbs->boot_tune_index.offset] = lbs->boot_tune_index.value;

  spi_flash_erase_sector(spi_sector_offset);

  for (i = 0; i < spi_page_size; i++) {
    spi_flash_write_byte(spi_crossbar_offset + i, crossbar[i]);
  }

  for (i = 0; i < spi_page_size; i++) {
    spi_flash_write_byte(spi_bios_settings_offset + i, bios_settings[i]);
  }
}