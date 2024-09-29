#include "llama/llama_boot.h"

#include "biosvar.h"  // GET_GLOBAL
#include "llama/llama_bios_settings.h"
#include "llama/llama_cs4237.h"
#include "llama/llama_emc2303.h"
#include "llama/llama_nbsb.h"
#include "llama/llama_speaker.h"
#include "malloc.h"  // LegacyRamSize
#include "output.h"  // dprintf
#include "util.h"    // mdelay
#include "x86.h"     // cr0_read, cr0_write, CR0_CD, CR0_NW, wbinvd

static u32 get_current_cpu_freq(void) {
  u32 strapreg2 = llama_nbsb_read32(vx86ex_nb, vx86ex_nbsb_func0, 0x64);
  u32 ddiv = (strapreg2 >> 14) & 0x01L;
  u32 cdiv = (strapreg2 >> 12) & 0x03L;
  u32 cms = (strapreg2 >> 8) & 0x03L;
  u32 cns = strapreg2 & 0xFFL;
  int crs = (int)((strapreg2 >> 10) & 0x03L);
  dprintf(1, "NS       = %d\n", (unsigned char)cns);
  dprintf(1, "MS       = %d\n", (unsigned char)cms);
  dprintf(1, "RS       = %d\n", crs);
  dprintf(1, "CPU_DIV  = %d\n", (unsigned char)cdiv);
  dprintf(1, "DRAM_DIV = %d\n", (unsigned char)ddiv);
  return (25L * cns) / (cms * (1L << crs) * (cdiv + 2L));
}

static u32 get_current_ram_size(void) {
  u32 ram_size_bytes = GET_GLOBAL(LegacyRamSize);
  u32 ram_size_kb = ram_size_bytes / 1024;
  u32 ram_size_mb = ram_size_kb / 1024;
  return ram_size_mb > 1800  ? 2048
         : ram_size_mb > 800 ? 1024
         : ram_size_mb > 450 ? 512
         : ram_size_mb > 200 ? 256
         : ram_size_mb > 100 ? 128
         : ram_size_mb > 50  ? 64
         : ram_size_mb > 20  ? 32
         : ram_size_mb > 10  ? 16
                             : 8;
}

void llama_boot(llama_bios_settings *lbs) {
  unsigned int cpu_freq = get_current_cpu_freq();
  unsigned int ram_size = get_current_ram_size();

  // Disable L1 cache if it's disabled in the BIOS settings
  if (lbs->l1_cache.value == 0) {
    setcr0(getcr0() | (CR0_CD | CR0_NW));
    wbinvd();
  }

  // Disable L2 cache if it's disabled in the BIOS settings
  if (lbs->l2_cache.value == 0) {
    u8 nb1_e8 = llama_nbsb_read8(vx86ex_nb, vx86ex_nbsb_func1, 0xE8);
    nb1_e8 &= ~0x03;  // clear bit 0 (L2_EN) and bit 1 (L2_WB_EN)
    llama_nbsb_write8(vx86ex_nb, vx86ex_nbsb_func1, 0xE8, nb1_e8);
  }

  // Set SBCLK
  outl(0x800038c0, 0x0cf8);
  u32 c0 = inl(0x0cfc);
  c0 |= (1 << 31);   // C0h[31]
  outl(c0, 0x0cfc);  // write value

  // Set UART clocks/dividers
  if (lbs->com1_base_clock_index.value > 0) {
    u32 com1cntr = inl(0x0c00);
    com1cntr |= (1 << 22);  // cntrl[22]
    if (lbs->com1_base_clock_index.value > 1) {
      com1cntr |= (1 << 20);  // cntrl[20]
    }
    outl(com1cntr, 0x0c00);
  }
  if (lbs->com2_base_clock_index.value > 0) {
    u32 com2cntr = inl(0x0c04);
    com2cntr |= (1 << 22);  // cntrl[22]
    if (lbs->com2_base_clock_index.value > 1) {
      com2cntr |= (1 << 20);  // cntrl[20]
    }
    outl(com2cntr, 0x0c04);
  }

  printf("\n");
  printf("CPU: DMP Vortex86EX     ");
  if (cpu_freq < 100) printf(" ");
  printf("%d MHz\n", cpu_freq);
  printf("RAM: DDR3              ");
  if (ram_size < 1000) printf(" ");
  printf("%d MB\n", ram_size);
  printf("L1 Cache: ");
  if (lbs->l1_cache.value) {
    printf("Enabled");
  } else {
    printf("Disabled");
  }
  printf("\n");
  printf("L2 Cache: ");
  if (lbs->l2_cache.value) {
    printf("Enabled");
  } else {
    printf("Disabled");
  }

  // First, initialize the CS4237 with external FM enabled
  cs4237_hostload(lbs, 1);
  cs4237_slam_configuration(lbs);
  lbs->cs4237_ext_fm_present = cs4237_test_opl3(lbs);
  if (lbs->cs4237_ext_fm.value == 0 || lbs->cs4237_ext_fm_present == 0) {
    // If the user has disabled external FM or it's not present,
    // initialize the CS4237 with internal FM enabled
    cs4237_hostload(lbs, 0);
    cs4237_slam_configuration(lbs);
  }
  cs4237_setup_mixer(lbs);
  if (lbs->cs4237_digital.value) {
    cs4237_enable_digital_output(lbs);
  }

  printf("\n\nCrystal CS4237 Initialized\nFM Synthesizer:        ");
  if (lbs->cs4237_ext_fm.value && lbs->cs4237_ext_fm_present) {
    printf("OPL3 module");
  } else {
    printf("Internal");
  }
  printf("\nDigital S/PDIF output: ");
  if (lbs->cs4237_digital.value) {
    printf("Enabled");
  } else {
    printf("Disabled");
  }

  printf("\n\nEMC2303 Fan Controller ");
  if (llama_get_emc_product_id_reg() == 0x35) {
    printf("initialized");
    llama_set_fan1_target_rpm(lbs->fan1_target_rpm.value);
    llama_set_fan2_target_rpm(lbs->fan2_target_rpm.value);
    llama_set_fan3_target_rpm(lbs->fan3_target_rpm.value);
  } else {
    printf("not found");
  }
  printf("\n\n");

  if (lbs->boot_tune_index.value == 1) {
    llama_play_ducks_tune();
  } else if (lbs->boot_tune_index.value == 2) {
    llama_play_mushroom_tune();
  }
}
