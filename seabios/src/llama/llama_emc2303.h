#ifndef __LLAMA_EMC2303_H
#define __LLAMA_EMC2303_H

#include "llama/llama_emc2303.h"
#include "types.h"

u8 llama_get_emc_product_id_reg(void);
//u8 llama_get_fan1_conf_reg(void);
//int llama_set_fan1_conf_reg(u8 reg_value);
//u8 llama_get_fan2_conf_reg(void);
//int llama_set_fan2_conf_reg(u8 reg_value);
//u8 llama_get_fan3_conf_reg(void);
//int llama_set_fan3_conf_reg(u8 reg_value);
u16 llama_get_fan1_tach_rpm(void);
u16 llama_get_fan2_tach_rpm(void);
u16 llama_get_fan3_tach_rpm(void);
//int llama_set_fan1_drive_reg(u8 reg_value);
//int llama_set_fan2_drive_reg(u8 reg_value);
//int llama_set_fan3_drive_reg(u8 reg_value);
int llama_set_fan1_target_rpm(u16 rpm);
int llama_set_fan2_target_rpm(u16 rpm);
int llama_set_fan3_target_rpm(u16 rpm);
void llama_get_fan_statuses(u8 *fan1, u8 *fan2, u8 *fan3);

#endif // __LLAMA_EMC2303_H