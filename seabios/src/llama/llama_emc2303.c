#include "llama/llama_emc2303.h"
#include "llama/llama_i2c.h"
#include "output.h" // printf

#define EMC2303_ADDR                0x2E

#define EMC2303_FAN_STALL_STATUS_REG 0x25
#define EMC2303_FAN_SPIN_STATUS_REG  0x26
#define EMC2303_FAN_FAIL_STATUS_REG  0x27

#define EMC2303_FAN1_DRIVE_REG      0x30
#define EMC2303_FAN2_DRIVE_REG      0x40
#define EMC2303_FAN3_DRIVE_REG      0x50

#define EMC2303_FAN1_CONF_REG       0x32
#define EMC2303_FAN2_CONF_REG       0x42
#define EMC2303_FAN3_CONF_REG       0x52

#define EMC2303_FAN1_CONF2_REG      0x33
#define EMC2303_FAN2_CONF2_REG      0x43
#define EMC2303_FAN3_CONF2_REG      0x53

#define EMC2303_FAN1_SPINUP_REG     0x36
#define EMC2303_FAN2_SPINUP_REG     0x46
#define EMC2303_FAN3_SPINUP_REG     0x56

#define EMC2303_FAN1_STEP_REG       0x37
#define EMC2303_FAN2_STEP_REG       0x47
#define EMC2303_FAN3_STEP_REG       0x57

#define EMC2303_FAN1_TARGET_TACH_HIGH_REG 0x3D
#define EMC2303_FAN1_TARGET_TACH_LOW_REG  0x3C
#define EMC2303_FAN2_TARGET_TACH_HIGH_REG 0x4D
#define EMC2303_FAN2_TARGET_TACH_LOW_REG  0x4C
#define EMC2303_FAN3_TARGET_TACH_HIGH_REG 0x5D
#define EMC2303_FAN3_TARGET_TACH_LOW_REG  0x5C

#define EMC2303_FAN1_MIN_DRIVE_REG  0x38
#define EMC2303_FAN2_MIN_DRIVE_REG  0x48
#define EMC2303_FAN3_MIN_DRIVE_REG  0x58

#define EMC2303_FAN1_TACH_HIGH_REG  0x3E
#define EMC2303_FAN1_TACH_LOW_REG   0x3F
#define EMC2303_FAN2_TACH_HIGH_REG  0x4E
#define EMC2303_FAN2_TACH_LOW_REG   0x4F
#define EMC2303_FAN3_TACH_HIGH_REG  0x5E
#define EMC2303_FAN3_TACH_LOW_REG   0x5F

#define EMC2303_PRODID_REG          0xFD

static inline u16 get_rpm(u8 tach_high, u8 tach_low)
{
  return (u16)(3932160 / (((tach_high << 8) | tach_low) >> 3));
}

static inline void get_regs_from_rpm(u8 *tach_high, u8 *tach_low, u16 rpm)
{
  u16 regs = rpm == 0 ? 0xffff : ((3932160 / rpm) << 3);
  *tach_high = regs >> 8;
  *tach_low = regs & 0xFF;
}

static u8 emc_get_reg_byte(u8 reg)
{
  u8 reg_value;
  if (i2c_start(EMC2303_ADDR, 0) &&
      i2c_write(&reg, 1) &&
      i2c_start(EMC2303_ADDR, 1) &&
      i2c_read(&reg_value, 1)) {

    return reg_value;
  }
  return 0;
}

static int emc_set_reg_byte(u8 reg, u8 reg_value)
{
  return i2c_start(EMC2303_ADDR, 0) &&
         i2c_write_byte_no_stop(reg) &&
         i2c_write(&reg_value, 1);
}

// Programming the RPM-based fan speed control algorithm:
// 1. Set the Spin Up Configuration Register to the spin up level and spin time desired.
// 2. Set the Fan Step Register to the desired step size.
// 3. Set the Fan Minimum Drive Register to the minimum drive value that will maintain fan operation.
// 4. Set the Update Time and Edges options in the Fan Configuration Register.
// 5. Set the Valid TACH Count Register to the highest tach count that indicates the fan is spinning.
//    Refer to AN17.4 RPM to TACH Counts Conversion for examples and tables for supported RPM ranges (500, 1k, 2k, 4k).
// 6. Set the TACH Target Register to the desired tachometer count.
// 7. Enable the RPM-based Fan Speed Control algorithm by setting the ENAG bit.
static int set_fanX_target_rpm(
  u8 step_reg,
  u8 min_drive_reg,
  u8 spinup_reg,
  u8 target_tach_low_reg,
  u8 target_tach_high_reg,
  u8 conf2_reg,
  u8 conf_reg,
  u16 rpm
) {
  u8 tach_high, tach_low = 0;
  if (rpm > 16000) {
    dprintf(1, "Invalid RPM value: %d\n", rpm);
    return 0;
  }
  get_regs_from_rpm(&tach_high, &tach_low, rpm);
  dprintf(1, "Setting tach_high: 0x%02x, tach_low: 0x%02x\n", tach_high, tach_low);

  return emc_set_reg_byte(step_reg, 0x04) &&
         emc_set_reg_byte(min_drive_reg, 0x00) &&
         emc_set_reg_byte(spinup_reg, 0x98) &&                 // disable after 16 failed updates, no kick, 30%, 2s
         emc_set_reg_byte(target_tach_low_reg, tach_low) &&
         emc_set_reg_byte(target_tach_high_reg, tach_high) &&
         emc_set_reg_byte(conf2_reg, 0x28) &&                  // Glitch filter enable, derivative options = basic, error window = 0rpm
         emc_set_reg_byte(conf_reg, 0x8B);                     // 7. reg[7] (ENAG) = 1 - enable RPM-based fan speed control
}

// static int llama_set_fan1_drive_reg(u8 reg_value)
// {
//   return emc_set_reg_byte(EMC2303_FAN1_DRIVE_REG, reg_value);
// }

// static int llama_set_fan2_drive_reg(u8 reg_value)
// {
//   return emc_set_reg_byte(EMC2303_FAN2_DRIVE_REG, reg_value);
// }

// static int llama_set_fan3_drive_reg(u8 reg_value)
// {
//   return emc_set_reg_byte(EMC2303_FAN3_DRIVE_REG, reg_value);
// }

// static u8 llama_get_fan1_conf_reg(void)
// {
//   return emc_get_reg_byte(EMC2303_FAN1_CONF_REG);
// }

// static int llama_set_fan1_conf_reg(u8 reg_value)
// {
//   return emc_set_reg_byte(EMC2303_FAN1_CONF_REG, reg_value);
// }

// static u8 llama_get_fan2_conf_reg(void)
// {
//   return emc_get_reg_byte(EMC2303_FAN2_CONF_REG);
// }

// static int llama_set_fan2_conf_reg(u8 reg_value)
// {
//   return emc_set_reg_byte(EMC2303_FAN2_CONF_REG, reg_value);
// }

// static u8 llama_get_fan3_conf_reg(void)
// {
//   return emc_get_reg_byte(EMC2303_FAN3_CONF_REG);
// }

// static int llama_set_fan3_conf_reg(u8 reg_value)
// {
//   return emc_set_reg_byte(EMC2303_FAN3_CONF_REG, reg_value);
// }

// Public functions

u8 llama_get_emc_product_id_reg(void)
{
  return emc_get_reg_byte(EMC2303_PRODID_REG);
}

u16 llama_get_fan1_tach_rpm(void)
{
  return get_rpm(
    emc_get_reg_byte(EMC2303_FAN1_TACH_HIGH_REG),
    emc_get_reg_byte(EMC2303_FAN1_TACH_LOW_REG)
  );
}

u16 llama_get_fan2_tach_rpm(void)
{
  return get_rpm(
    emc_get_reg_byte(EMC2303_FAN2_TACH_HIGH_REG),
    emc_get_reg_byte(EMC2303_FAN2_TACH_LOW_REG)
  );
}

u16 llama_get_fan3_tach_rpm(void)
{
  return get_rpm(
    emc_get_reg_byte(EMC2303_FAN3_TACH_HIGH_REG),
    emc_get_reg_byte(EMC2303_FAN3_TACH_LOW_REG)
  );
}

int llama_set_fan1_target_rpm(u16 rpm)
{
  return set_fanX_target_rpm(
    EMC2303_FAN1_STEP_REG,
    EMC2303_FAN1_MIN_DRIVE_REG,
    EMC2303_FAN1_SPINUP_REG,
    EMC2303_FAN1_TARGET_TACH_LOW_REG,
    EMC2303_FAN1_TARGET_TACH_HIGH_REG,
    EMC2303_FAN1_CONF2_REG,
    EMC2303_FAN1_CONF_REG,
    rpm
  );
}

int llama_set_fan2_target_rpm(u16 rpm)
{
  return set_fanX_target_rpm(
    EMC2303_FAN2_STEP_REG,
    EMC2303_FAN2_MIN_DRIVE_REG,
    EMC2303_FAN2_SPINUP_REG,
    EMC2303_FAN2_TARGET_TACH_LOW_REG,
    EMC2303_FAN2_TARGET_TACH_HIGH_REG,
    EMC2303_FAN2_CONF2_REG,
    EMC2303_FAN2_CONF_REG,
    rpm
  );
}

int llama_set_fan3_target_rpm(u16 rpm)
{
  return set_fanX_target_rpm(
    EMC2303_FAN3_STEP_REG,
    EMC2303_FAN3_MIN_DRIVE_REG,
    EMC2303_FAN3_SPINUP_REG,
    EMC2303_FAN3_TARGET_TACH_LOW_REG,
    EMC2303_FAN3_TARGET_TACH_HIGH_REG,
    EMC2303_FAN3_CONF2_REG,
    EMC2303_FAN3_CONF_REG,
    rpm
  );
}

// bit 0 = stall, bit 1 = spin-up failure, bit 2 = drive fail
void llama_get_fan_statuses(u8 *fan1, u8 *fan2, u8 *fan3)
{
  u8 stall = emc_get_reg_byte(EMC2303_FAN_STALL_STATUS_REG);
  u8 spin  = emc_get_reg_byte(EMC2303_FAN_SPIN_STATUS_REG);
  u8 fail  = emc_get_reg_byte(EMC2303_FAN_FAIL_STATUS_REG);

  *fan1 = (stall & 0x01) | ((spin & 0x01) << 1) | ((fail & 0x01) << 2);
  *fan2 = ((stall & 0x02) >> 1) | (spin & 0x02) | ((fail & 0x02) << 1);
  *fan3 = ((stall & 0x04) >> 2) | ((spin & 0x04) >> 1) | (fail & 0x04);
}