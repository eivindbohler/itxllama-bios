#ifndef __LLAMA_BIOS_MENU_HANDLERS_H
#define __LLAMA_BIOS_MENU_HANDLERS_H

#include "llama/llama_bios_settings.h"
#include "llama/llama_bios_menu.h"
#include "types.h"

u16 handler_get_cpu_freq_index(llama_bios_settings *lbs);
u16 handler_get_l1_cache(llama_bios_settings *lbs);
u16 handler_get_l2_cache(llama_bios_settings *lbs);
u16 handler_get_boot_order_index(llama_bios_settings *lbs);
u16 handler_get_usb_as_fixed_disks(llama_bios_settings *lbs);
u16 handler_get_com1_base_clock_index(llama_bios_settings *lbs);
u16 handler_get_com2_base_clock_index(llama_bios_settings *lbs);
u16 handler_get_cs4237_wss_port(llama_bios_settings *lbs);
u16 handler_get_cs4237_syn_port(llama_bios_settings *lbs);
u16 handler_get_cs4237_sb_port(llama_bios_settings *lbs);
u16 handler_get_cs4237_gamepad_port(llama_bios_settings *lbs);
u16 handler_get_cs4237_mpu_port(llama_bios_settings *lbs);
u16 handler_get_cs4237_ext_fm(llama_bios_settings *lbs);
u16 handler_get_cs4237_digital(llama_bios_settings *lbs);
u16 handler_get_mac_address_index(llama_bios_settings *lbs);
u16 handler_get_fan1_target_rpm(llama_bios_settings *lbs);
u16 handler_get_fan2_target_rpm(llama_bios_settings *lbs);
u16 handler_get_fan3_target_rpm(llama_bios_settings *lbs);
u16 handler_get_boot_tune_index(llama_bios_settings *lbs);
u16 handler_get_leds_enabled(llama_bios_settings *lbs);
u16 handler_get_leds_intensity(llama_bios_settings *lbs);
u16 handler_get_click_enabled(llama_bios_settings *lbs);

void handler_set_cpu_freq_index(llama_bios_settings *lbs, u16 value);
void handler_set_l1_cache(llama_bios_settings *lbs, u16 value);
void handler_set_l2_cache(llama_bios_settings *lbs, u16 value);
void handler_set_boot_order_index(llama_bios_settings *lbs, u16 value);
void handler_set_usb_as_fixed_disks(llama_bios_settings *lbs, u16 value);
void handler_set_com1_base_clock_index(llama_bios_settings *lbs, u16 value);
void handler_set_com2_base_clock_index(llama_bios_settings *lbs, u16 value);
void handler_set_cs4237_wss_port(llama_bios_settings *lbs, u16 value);
void handler_set_cs4237_syn_port(llama_bios_settings *lbs, u16 value);
void handler_set_cs4237_sb_port(llama_bios_settings *lbs, u16 value);
void handler_set_cs4237_gamepad_port(llama_bios_settings *lbs, u16 value);
void handler_set_cs4237_mpu_port(llama_bios_settings *lbs, u16 value);
void handler_set_cs4237_ext_fm(llama_bios_settings *lbs, u16 value);
void handler_set_cs4237_digital(llama_bios_settings *lbs, u16 value);
void handler_set_mac_address_index(llama_bios_settings *lbs, u16 value);
void handler_set_fan1_target_rpm(llama_bios_settings *lbs, u16 value);
void handler_set_fan2_target_rpm(llama_bios_settings *lbs, u16 value);
void handler_set_fan3_target_rpm(llama_bios_settings *lbs, u16 value);
void handler_set_boot_tune_index(llama_bios_settings *lbs, u16 value);
void handler_set_leds_enabled(llama_bios_settings *lbs, u16 value);
void handler_set_leds_intensity(llama_bios_settings *lbs, u16 value);
void handler_set_click_enabled(llama_bios_settings *lbs, u16 value);

#endif // __LLAMA_BIOS_MENU_HANDLERS_H