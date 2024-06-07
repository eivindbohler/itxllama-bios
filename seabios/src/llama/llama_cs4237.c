#include "llama/llama_cs4237.h"
#include "llama/llama_bios_settings.h"
#include "types.h"  // u8
#include "x86.h"    // outb
#include "util.h"   // udelay
#include "output.h" // dprintf

#define CTRLBASE_HIGH 0x05
#define CTRLBASE_LOW  0x38
#define CTRLBASE      ((CTRLBASE_HIGH << 8) | CTRLBASE_LOW)
#define WSSBASE_HIGH  0x05
#define WSSBASE_LOW   0x34
#define WSSBASE       ((WSSBASE_HIGH << 8) | WSSBASE_LOW)
#define SYNBASE_HIGH  0x03
#define SYNBASE_LOW   0x88
#define SYNBASE       ((SYNBASE_HIGH << 8) | SYNBASE_LOW)
#define SBBASE_HIGH   0x02
#define SBBASE_LOW    0x20
#define SBBASE        ((SBBASE_HIGH << 8) | SBBASE_LOW))
#define GAMEBASE_HIGH 0x02
#define GAMEBASE_LOW  0x00
#define GAMEBASE      ((GAMEBASE_HIGH << 8) | GAMEBASE_LOW)
#define MPUBASE_HIGH  0x03
#define MPUBASE_LOW   0x30
#define MPUBASE       ((MPUBASE_HIGH << 8) | MPUBASE_LOW)
#define CONF_LENGTH   178
#define SLAM_LENGTH    41
#define UDELAY        500

typedef enum {
  CTRL_C0, CTRL_C1, CTRL_C2, CTRL_C3,
  CTRL_C4, CTRL_C5, CTRL_C6, CTRL_C7,
  CTRL_C8
} ctrl_indirect_reg_t;

typedef enum {
  WSS_R0, WSS_R1, WSS_R2, WSS_R3
} wss_direct_reg_t;

typedef enum {
  WSS_I0,  WSS_I1,  WSS_I2,  WSS_I3,
  WSS_I4,  WSS_I5,  WSS_I6,  WSS_I7,
  WSS_I8,  WSS_I9,  WSS_I10, WSS_I11,
  WSS_I12, WSS_I13, WSS_I14, WSS_I15,
  WSS_I16, WSS_I17, WSS_I18, WSS_I19,
  WSS_I20, WSS_I21, WSS_I22, WSS_I23,
  WSS_I24, WSS_I25, WSS_I26, WSS_I27,
  WSS_I28, WSS_I29, WSS_I30, WSS_I31
} wss_indirect_reg_t;

typedef enum {
  WSS_X0,  WSS_X1,  WSS_X2,  WSS_X3,
  WSS_X4,  WSS_X5,  WSS_X6,  WSS_X7,
  WSS_X8,  WSS_X9,  WSS_X10, WSS_X11,
  WSS_X12, WSS_X13, WSS_X14, WSS_X15,
  WSS_X16, WSS_X17, WSS_X18, WSS_X19,
  WSS_X20, WSS_X21, WSS_X22, WSS_X23,
  WSS_X24, WSS_X25
} wss_ext_reg_t;

const u8 crystal_key[32] = {
  0x96, 0x35, 0x9A, 0xCD, 0xE6, 0xF3, 0x79, 0xBC,
  0x5E, 0xAF, 0x57, 0x2B, 0x15, 0x8A, 0xC5, 0xE2,
  0xF1, 0xF8, 0x7C, 0x3E, 0x9F, 0x4F, 0x27, 0x13,
  0x09, 0x84, 0x42, 0xA1, 0xD0, 0x68, 0x34, 0x1A
};

const u8 ctrl_base_conf[8] = {
  0x15, 0x02, 0x47,
  CTRLBASE_HIGH, // index: 3
  CTRLBASE_LOW,  // index: 4
  0x33, 0x01, 0x79
};

const u8 cs4237_conf[CONF_LENGTH] = {
  0x00, 0x03, 0x80,
  0x00, // index: 3 = 0x80 = internal FM, 0x00 = external FM
  0x0B, 0x20, 0x04, 0x08, 0x10, 0x80, 0x00, 0x00, 0x00, 0x48, 0x75, 0xB9,
  0xFC, 0x10, 0x03, 0x0E, 0x63, 0x42, 0x37, 0xFF, 0xFF, 0xFF, 0xFF, 0x30, 0x0A, 0x10, 0x01, 0x82,
  0x12, 0x00, 0x49, 0x54, 0x58, 0x2D, 0x4C, 0x6C, 0x61, 0x6D, 0x61, 0x2D, 0x43, 0x53, 0x34, 0x32,
  0x33, 0x37, 0x42, 0x00, 0x15, 0x0E, 0x63, 0x00, 0x00, 0x00, 0x82, 0x07, 0x00, 0x57, 0x53, 0x53,
  0x2F, 0x53, 0x42, 0x00, 0x31, 0x00, 0x2A, 0x02, 0x28, 0x22, 0x80, 0x00, 0x47, 0x01,
  WSSBASE_LOW, WSSBASE_HIGH, // index: 78, 79 = WSSbase, low, high
  WSSBASE_LOW, WSSBASE_HIGH, // index: 80, 81 = WSSbase, low, high
  0x04, 0x04, 0x47, 0x01,
  SYNBASE_LOW, SYNBASE_HIGH, // index: 86, 87 = SYNbase, low, high
  SYNBASE_LOW, SYNBASE_HIGH, // index: 88, 89 = SYNbase, low, high
  0x08, 0x04, 0x47, 0x01,
  SBBASE_LOW, SBBASE_HIGH, // index: 94, 95 = SBbase, low, high
  SBBASE_LOW, SBBASE_HIGH, // index: 96, 97 = SBbase, low, high
  0x20, 0x10, 0x38, 0x15, 0x0E, 0x63, 0x00, 0x01, 0x00, 0x82, 0x05, 0x00, 0x47, 0x41,
  0x4D, 0x45, 0x00, 0x31, 0x00, 0x47, 0x01,
  GAMEBASE_LOW, GAMEBASE_HIGH, // index: 119, 120 = GAMEbase, low, high
  GAMEBASE_LOW, GAMEBASE_HIGH, // index: 121, 122 = GAMEbase, low, high
  0x08, 0x08, 0x38, 0x15, 0x0E,
  0x63, 0x00, 0x10, 0x00, 0x82, 0x05, 0x00, 0x43, 0x54, 0x52, 0x4C, 0x00, 0x47, 0x01,
  CTRLBASE_LOW, CTRLBASE_HIGH, // index: 142, 143 = CTRLbase, low, high
  CTRLBASE_LOW, CTRLBASE_HIGH, // index: 144, 145 = CTRLbase, low, high
  0x08, 0x08, 0x15, 0x0E, 0x63, 0x00, 0x03, 0x00, 0x82, 0x08, 0x00, 0x4D, 0x50, 0x55,
  0x2D, 0x34, 0x30, 0x31, 0x00, 0x31, 0x00, 0x47, 0x01,
  MPUBASE_LOW, MPUBASE_HIGH, // index: 169, 170 = MPUbase, low, high
  MPUBASE_LOW, MPUBASE_HIGH, // index: 171, 172 = MPUbase, low, high
  0x08, 0x02, 0x38, 0x79, 0xEA
};

const u8 slam_conf[SLAM_LENGTH] = {
  0x06, 0x00,                        // CSN = 0
  0x15, 0x00,                        // Logical Device 0
  0x47, WSSBASE_HIGH, WSSBASE_LOW,   // index  5,  6 = WSSbase (default: 0x534)
  0x48, SYNBASE_HIGH, SYNBASE_LOW,   // index  8,  9 = SYNbase (default: 0x388)
  0x42, SBBASE_HIGH, SBBASE_LOW,     // index 11, 12 = SBbase  (default: 0x220)
  0x22, 0x07,                        // WSS & SB IRQ = 7
  0x2a, 0x01,                        // WSS & SB DMA = 1
  0x33, 0x01,                        // Activate logical device 0
  0x15, 0x01,                        // Logical Device 1
  0x47, GAMEBASE_HIGH, GAMEBASE_LOW, // index 22, 23 = GAMEbase (default: 0x200)
  0x33, 0x01,                        // Activate logical device 1
  0x15, 0x02,                        // Logical Device 2
  0x47, CTRLBASE_HIGH, CTRLBASE_LOW, // index 29, 30 = CTRLbase (default: 0x538)
  0x33, 0x01,                        // Activate logical device 2
  0x15, 0x03,                        // Logical Device 3
  0x47, MPUBASE_HIGH, MPUBASE_LOW,   // index 36, 37 = MPUbase (default: 0x330)
  0x33, 0x01,                        // Activate logical device 3
  0x79                               // Activate Crystal device
};

// static u8 ctrl_indirect_read(ctrl_indirect_reg_t reg)
// {
//   u8 val;
//   outb(reg, CTRLBASE + 3);
//   udelay(UDELAY);
//   val = inb(CTRLBASE + 4);
//   udelay(UDELAY);
//   return val;
// }

static void ctrl_indirect_write(ctrl_indirect_reg_t reg, u8 val)
{
  outb(reg, CTRLBASE + 3);
  udelay(UDELAY);
  outb(val, CTRLBASE + 4);
  udelay(UDELAY);
}

static void wss_set_mce(llama_bios_settings *lbs)
{
  u16 wss_base = lbs->cs4237_wss_port.value;
  outb((inb(wss_base + WSS_R0) | 0x40), wss_base + WSS_R0);
  udelay(UDELAY);
}

static void wss_clear_mce(llama_bios_settings *lbs)
{
  u16 wss_base = lbs->cs4237_wss_port.value;
  outb((inb(wss_base + WSS_R0) & ~0x40), wss_base + WSS_R0);
  udelay(UDELAY);
}

// static u8 wss_direct_read(wss_direct_reg_t reg)
// {
//   u8 val = inb(WSSBASE + reg);
//   udelay(UDELAY);
//   return val;
// }

// static void wss_direct_write(wss_direct_reg_t reg, u8 val)
// {
//   outb(val, WSSBASE + reg);
//   udelay(UDELAY);
// }

static u8 wss_indirect_read(wss_indirect_reg_t reg, llama_bios_settings *lbs)
{
  u8 val;
  u16 wss_base = lbs->cs4237_wss_port.value;
  outb(((inb(wss_base + WSS_R0) & 0x60) | (reg & 0x1F)), wss_base + WSS_R0);
  udelay(UDELAY);
  val = inb(wss_base + WSS_R1);
  udelay(UDELAY);
  return val;
}

static void wss_indirect_write(wss_indirect_reg_t reg, u8 val, llama_bios_settings *lbs)
{
  u16 wss_base = lbs->cs4237_wss_port.value;
  outb(((inb(wss_base + WSS_R0) & 0x60) | (reg & 0x1F)), wss_base + WSS_R0);
  udelay(UDELAY);
  outb(val, wss_base + WSS_R1);
  udelay(UDELAY);
}

// MODE 3 only
static u8 wss_ext_read(wss_ext_reg_t reg, llama_bios_settings *lbs)
{
  u8 val;
  u16 wss_base = lbs->cs4237_wss_port.value;
  u8 I23val = (((reg & 0x10) >> 2) & ((reg & 0x0F) << 4)) | 0x08;
  wss_indirect_write(WSS_I23, I23val, lbs);
  val = inb(wss_base + WSS_R1);
  udelay(UDELAY);
  return val;
}

// MODE 3 only
static void wss_ext_write(wss_ext_reg_t reg, u8 val, llama_bios_settings *lbs)
{
  u16 wss_base = lbs->cs4237_wss_port.value;
  u8 I23val = (((reg & 0x10) >> 2) & ((reg & 0x0F) << 4)) | 0x08;
  wss_indirect_write(WSS_I23, I23val, lbs);
  outb(val, wss_base + WSS_R1);
  udelay(UDELAY);
}

void cs4237_hostload(llama_bios_settings *lbs, u8 force_ext_fm)
{
  int i = 0;
  u8 cs4237_conf_temp[CONF_LENGTH];
  for (i = 0; i < CONF_LENGTH; i++) {
    cs4237_conf_temp[i] = cs4237_conf[i];
  }

  // send crystal key to 0x279
  for (i = 0; i < 32; i++) {
    outb(crystal_key[i], 0x279);
    udelay(UDELAY);
  }

  // set CTRLbase
  for (i = 0; i < 8; i++) {
    outb(ctrl_base_conf[i], 0x279);
    udelay(UDELAY);
  }

  // jump to ROM
  outb(0x57, CTRLBASE + 0x05);
  udelay(UDELAY);

  // send download command
  outb(0xAA, CTRLBASE + 0x05);
  udelay(UDELAY);

  // send download address
  outb(0x00, CTRLBASE + 0x05);
  udelay(UDELAY);
  outb(0x40, CTRLBASE + 0x05);
  udelay(UDELAY);

  // Prepare data by inserting values from BIOS settings
  cs4237_conf_temp[3]  = (force_ext_fm || lbs->cs4237_ext_fm.value) ? 0x00 : 0x80;
  cs4237_conf_temp[78] = (lbs->cs4237_wss_port.value & 0xFF);
  cs4237_conf_temp[79] = (lbs->cs4237_wss_port.value >> 8);
  cs4237_conf_temp[80] = (lbs->cs4237_wss_port.value & 0xFF);
  cs4237_conf_temp[81] = (lbs->cs4237_wss_port.value >> 8);
  cs4237_conf_temp[86] = (lbs->cs4237_syn_port.value & 0xFF);
  cs4237_conf_temp[87] = (lbs->cs4237_syn_port.value >> 8);
  cs4237_conf_temp[88] = (lbs->cs4237_syn_port.value & 0xFF);
  cs4237_conf_temp[89] = (lbs->cs4237_syn_port.value >> 8);
  cs4237_conf_temp[94] = (lbs->cs4237_sb_port.value & 0xFF);
  cs4237_conf_temp[95] = (lbs->cs4237_sb_port.value >> 8);
  cs4237_conf_temp[96] = (lbs->cs4237_sb_port.value & 0xFF);
  cs4237_conf_temp[97] = (lbs->cs4237_sb_port.value >> 8);
  cs4237_conf_temp[119] = (lbs->cs4237_gamepad_port.value & 0xFF);
  cs4237_conf_temp[120] = (lbs->cs4237_gamepad_port.value >> 8);
  cs4237_conf_temp[121] = (lbs->cs4237_gamepad_port.value & 0xFF);
  cs4237_conf_temp[122] = (lbs->cs4237_gamepad_port.value >> 8);
  cs4237_conf_temp[169] = (lbs->cs4237_mpu_port.value & 0xFF);
  cs4237_conf_temp[170] = (lbs->cs4237_mpu_port.value >> 8);
  cs4237_conf_temp[171] = (lbs->cs4237_mpu_port.value & 0xFF);
  cs4237_conf_temp[172] = (lbs->cs4237_mpu_port.value >> 8);

  // send data
  for (i = 0; i < CONF_LENGTH; i++) {
    outb(cs4237_conf_temp[i], CTRLBASE + 0x05);
    udelay(UDELAY);
  }

  // end download
  outb(0x00, CTRLBASE + 0x06);
  udelay(UDELAY);

  // force internal update
  outb(0x5A, CTRLBASE + 0x05);
  udelay(UDELAY);
}

void cs4237_slam_configuration(llama_bios_settings *lbs)
{
  int i;
  u8 slam_conf_temp[SLAM_LENGTH];
  for (i = 0; i < SLAM_LENGTH; i++) {
    slam_conf_temp[i] = slam_conf[i];
  }

  // Enter configuration mode by sending Crystal key
  for (i = 0; i < 32; i++) {
    outb(crystal_key[i], 0x279);
    udelay(UDELAY);
  }

  // Prepare data by inserting values from BIOS settings
  slam_conf_temp[5] = (lbs->cs4237_wss_port.value >> 8);
  slam_conf_temp[6] = (lbs->cs4237_wss_port.value & 0xFF);
  slam_conf_temp[8] = (lbs->cs4237_syn_port.value >> 8);
  slam_conf_temp[9] = (lbs->cs4237_syn_port.value & 0xFF);
  slam_conf_temp[11] = (lbs->cs4237_sb_port.value >> 8);
  slam_conf_temp[12] = (lbs->cs4237_sb_port.value & 0xFF);
  slam_conf_temp[22] = (lbs->cs4237_gamepad_port.value >> 8);
  slam_conf_temp[23] = (lbs->cs4237_gamepad_port.value & 0xFF);
  slam_conf_temp[36] = (lbs->cs4237_mpu_port.value >> 8);
  slam_conf_temp[37] = (lbs->cs4237_mpu_port.value & 0xFF);

  // Activate logical devices using SLAM
  for (i = 0; i < SLAM_LENGTH; i++) {
    outb(slam_conf_temp[i], 0x279);
    udelay(UDELAY);
  }
}

void cs4237_setup_mixer(llama_bios_settings *lbs)
{
  // Set signal routing so that all sources pass from analogue output
  // mixer, through the output loopback, and into the analogue input
  // mixer to the ADC.  We are also carefully muting all sources
  // feeding directly into the analogue input mixer.  We are not paying
  // attention to volume levels, and are instead setting everything to
  // no attenuation, no amplification.

  // Set MODE 3
  wss_indirect_write(WSS_I12, 0xEA, lbs);

  // Mute DSP serial port input.
  wss_ext_write(WSS_X8, 0x80, lbs);
  wss_ext_write(WSS_X9, 0x80, lbs);

  // Completely mute microphone.
  wss_ext_write(WSS_X2, 0xDF, lbs);
  wss_ext_write(WSS_X3, 0xDF, lbs);

  // Set AUX1 (line in) routing.
  wss_indirect_write(WSS_I2, 0x68, lbs);
  wss_indirect_write(WSS_I3, 0x68, lbs);

  // Set AUX2 (CD audio) routing.
  wss_indirect_write(WSS_I4, 0x48, lbs);
  wss_indirect_write(WSS_I5, 0x48, lbs);

  // Set LINE (analogue FM) routing.
  wss_ext_write(WSS_X0, 0x68, lbs);
  wss_ext_write(WSS_X1, 0x68, lbs);

  // Set Crystal FM volume level.  This is optional.
  wss_ext_write(WSS_X6, 0x00, lbs);
  wss_ext_write(WSS_X7, 0x00, lbs);

  // Mute master digital output through input mixer.
  wss_ext_write(WSS_X11, 0xC0, lbs);

  // Set master digital volume.
  wss_ext_write(WSS_X14, 0x00, lbs);
  wss_ext_write(WSS_X15, 0x00, lbs);

  // Make sure we're not attenuating at the analogue input mixer.
  wss_ext_write(WSS_X4, (wss_ext_read(WSS_X4, lbs) & 0x9C), lbs);
  wss_ext_write(WSS_X5, (wss_ext_read(WSS_X5, lbs) & 0x80), lbs);

  // Enable output loopback so that *all* analogue sources pass
  // through ADC, and subsequently to serial port (because of 3DSO).
  // This also sets no attenuation of input mixer signal to ADC.
  wss_indirect_write(WSS_I0, 0xC0, lbs);
  wss_indirect_write(WSS_I1, 0xC0, lbs);
}

void cs4237_enable_digital_output(llama_bios_settings *lbs)
{
  // Set channel status and format flags.
  ctrl_indirect_write(CTRL_C5, 0x24);
  ctrl_indirect_write(CTRL_C6, 0x00);

  // Select ADC as source for consumer serial output, disable SRS.
  ctrl_indirect_write(CTRL_C3, 0x00);

  // Set CSPE; other bits are not important yet.
  ctrl_indirect_write(CTRL_C4, 0x80);

  // Enable serial port via I16 (SPE bit), preserving other bits.
  // MCE must be set for this to have any effect.
  wss_set_mce(lbs);
  wss_indirect_write(WSS_I16, (wss_indirect_read(WSS_I16, lbs) | 0x02), lbs);
  wss_clear_mce(lbs);

  // Set, then clear CSBR.
  // This is necessary to ensure reliable,
  // consistent locking on many decoder/DAC/receiver units.
  ctrl_indirect_write(CTRL_C4, 0xC0);
  ctrl_indirect_write(CTRL_C4, 0x80);
}

// This method tests the presence of an OPL3 device.
// Make sure to set the card to external FM synth first,
// if testing for an external OPL3 module.
u8 cs4237_test_opl3(llama_bios_settings *lbs)
{
  u8 test1, test2;

  // Do the OPL3 detection routine
  outb(0x04, (lbs->cs4237_syn_port.value));        // Select register 4
  udelay(UDELAY);
  outb(0x60, (lbs->cs4237_syn_port.value + 0x01)); // Reset Timer 1 and Timer 2
  udelay(UDELAY);
  outb(0x80, (lbs->cs4237_syn_port.value + 0x01)); // Reset the IRQ
  udelay(UDELAY);
  test1 = inb(lbs->cs4237_syn_port.value);         // Read the status register

  outb(0x02, (lbs->cs4237_syn_port.value));        // Select register 2
  udelay(UDELAY);
  outb(0xFF, (lbs->cs4237_syn_port.value + 0x01)); // Set Timer 1 to 0xFF
  udelay(UDELAY);
  outb(0x04, (lbs->cs4237_syn_port.value));        // Select register 4
  udelay(UDELAY);
  outb(0x21, (lbs->cs4237_syn_port.value + 0x01)); // Unmask and start Timer 1
  udelay(UDELAY);
  test2 = inb(lbs->cs4237_syn_port.value);         // Read the status register
  outb(0x60, (lbs->cs4237_syn_port.value + 0x01)); // Reset Timer 1 and Timer 2
  udelay(UDELAY);
  outb(0x80, (lbs->cs4237_syn_port.value + 0x01)); // Reset the IRQ
  udelay(UDELAY);

  test1 &= 0xE0; // Only care about the IRQFlag, T1Flag, and T2Flag
  test2 &= 0xE0;
  dprintf(1, "\nOPL3 detection - test1 byte: 0x%02x, test2 byte: 0x%02x\n", test1, test2);
  return (test1 == 0x00 && test2 == 0xc0);
}