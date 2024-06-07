#include "llama/llama_strings.h"
#include "types.h"

const char *get_string(u16 id)
{
  switch (id) {
    case STRING_MAIN_CPU_SETTINGS:
      return "CPU Settings";
    case STRING_NAME_CPU_FREQ:
      return "CPU Frequency";
    case STRING_DESC_CPU_FREQ_1:
      return "A low CPU frequency makes sense for use with early";
    case STRING_DESC_CPU_FREQ_2:
      return "80s programs and games - and draws less power.";
    case STRING_DESC_CPU_FREQ_3:
      return "A high frequency runs hotter and requires active cooling.";
    case STRING_NAME_L1_CACHE:
      return "L1 Cache";
    case STRING_DESC_L1_CACHE_1:
      return "Disabling the CPU L1 Cache slows down the system significantly.";
    case STRING_DESC_L1_CACHE_2:
    case STRING_DESC_L2_CACHE_2:
      return "Only use this setting if you require 286-like performance for";
    case STRING_DESC_L1_CACHE_3:
    case STRING_DESC_L2_CACHE_3:
      return "speed-sensitive, early 80s games.";
    case STRING_NAME_L2_CACHE:
      return "L2 Cache";
    case STRING_DESC_L2_CACHE_1:
      return "Disabling the CPU L2 Cache slows down the system significantly.";
    case STRING_MAIN_DISK_SETTINGS:
      return "Disk Settings";
    case STRING_NAME_BOOT_ORDER:
      return "Boot Order";
    case STRING_DESC_BOOT_ORDER_1:
      return "Select the order in which the system tries to boot from the";
    case STRING_DESC_BOOT_ORDER_2:
      return "available boot devices. Pressing ESC during boot lets you";
    case STRING_DESC_BOOT_ORDER_3:
      return "select a device manually.";
    case STRING_NAME_USB_AS_FIXED_DISKS:
      return "USB as Fixed Disks";
    case STRING_DESC_USB_AS_FIXED_DISKS_1:
      return "Enabling this setting makes USB storage devices appear as fixed";
    case STRING_DESC_USB_AS_FIXED_DISKS_2:
      return "disks to the system. Needed to boot from USB devices.";
    case STRING_MAIN_COM_SETTINGS:
      return "COM Settings";
    case STRING_NAME_COM1_BASE_CLOCK:
      return "COM1 Base Clock";
    case STRING_DESC_COM1_BASE_CLOCK_1:
    case STRING_DESC_COM2_BASE_CLOCK_1:
      return "A baud rate of 115200 has a divider of 1. 57600 = 2, 38400 = 3, etc.";
    case STRING_DESC_COM1_BASE_CLOCK_2:
    case STRING_DESC_COM2_BASE_CLOCK_2:
      return "For example, setting your baud rate to 19200 with a base clock of 3 MHz";
    case STRING_DESC_COM1_BASE_CLOCK_3:
    case STRING_DESC_COM2_BASE_CLOCK_3:
      return "gives you an actual baud rate of 3 MHz / 6 = 500000 baud";
    case STRING_NAME_COM2_BASE_CLOCK:
      return "COM2 Base Clock";
    case STRING_MAIN_AUDIO_SETTINGS:
      return "Audio Settings";
    case STRING_NAME_WSS_PORT:
      return "WSS Port Address";
    case STRING_DESC_WSS_PORT_1:
      return "Port address for the CS4237 Windows Sound System Codec.";
    case STRING_DESC_WSS_PORT_2:
    case STRING_DESC_SYN_PORT_2:
    case STRING_DESC_SB_PORT_2:
    case STRING_DESC_GAMEPAD_PORT_2:
    case STRING_DESC_MPU_PORT_2:
      return "As a general rule, you don't need to change this.";
    case STRING_NAME_SYN_PORT:
      return "FM Synth Port Address";
    case STRING_DESC_SYN_PORT_1:
      return "Port address for the CS4237 FM Synthesizer.";
    case STRING_NAME_SB_PORT:
      return "Sound Blaster Port Address";
    case STRING_DESC_SB_PORT_1:
      return "Port address for the CS4237 Sound Blaster.";
    case STRING_NAME_GAMEPAD_PORT:
      return "Gamepad Port Address";
    case STRING_DESC_GAMEPAD_PORT_1:
      return "Port address for the CS4237 Gamepad/Joystick.";
    case STRING_NAME_MPU_PORT:
      return "MPU-401 Port Address";
    case STRING_DESC_MPU_PORT_1:
      return "Port address for the CS4237 MPU-401.";
    case STRING_NAME_EXT_FM:
      return "External FM Synthesizer";
    case STRING_DESC_EXT_FM_1:
      return "Use the \"Llama OPL-3 module\" external FM Synthesizer";
    case STRING_DESC_EXT_FM_2:
      return "instead of the built-in FM synth on the CS4237.";
    case STRING_DESC_EXT_FM_3:
      return "Not selectable if the module is not installed.";
    case STRING_NAME_DIGITAL:
      return "S/PDIF Digital Output";
    case STRING_DESC_DIGITAL_2:
      return "Enabling this setting turns on the digital optical output.";
    case STRING_MAIN_ETHERNET_SETTINGS:
      return "Ethernet Settings";
    case STRING_NAME_MAC_ADDRESS:
      return "MAC Address";
    case STRING_DESC_MAC_ADDRESS_2:
      return "Select the MAC address to use for the Ethernet adapter.";
    case STRING_MAIN_FAN_CONTROL:
      return "Fan Control";
    case STRING_NAME_FAN1_TARGET_RPM:
      return "Fan 1 Target RPM";
    case STRING_DESC_FAN1_TARGET_RPM_1:
    case STRING_DESC_FAN2_TARGET_RPM_1:
    case STRING_DESC_FAN3_TARGET_RPM_1:
      return "Set the target RPM for the fans. Note that setting this";
    case STRING_DESC_FAN1_TARGET_RPM_2:
    case STRING_DESC_FAN2_TARGET_RPM_2:
    case STRING_DESC_FAN3_TARGET_RPM_2:
      return "too low may cause the fan to stall and oscillate.";
    case STRING_NAME_FAN2_TARGET_RPM:
      return "Fan 2 Target RPM";
    case STRING_NAME_FAN3_TARGET_RPM:
      return "Fan 3 Target RPM";
    case STRING_MAIN_MISC_SETTINGS:
      return "Misc Settings";
    case STRING_NAME_BOOT_TUNE:
      return "Boot Tune";
    case STRING_DESC_BOOT_TUNE_2:
      return "Select or disable playing a short tune when booting the system.";
    case STRING_NAME_LEDS_ENABLED:
      return "LEDs Enabled";
    case STRING_DESC_LEDS_ENABLED_2:
      return "Enable or disable the standby-, power- and HDD activity LEDs.";
    case STRING_NAME_LEDS_INTENSITY:
      return "LEDs Intensity";
    case STRING_DESC_LEDS_INTENSITY_2:
      return "Set the intensity of the LEDs.";
    case STRING_NAME_CLICK_ENABLED:
      return "Clicker Enabled";
    case STRING_DESC_CLICK_ENABLED_1:
      return "Enable or disable a clicking sound on SD activity";
    case STRING_DESC_CLICK_ENABLED_2:
      return "that emulates the sound of a mechincal hard drive.";

    case STRING_NAME_QUIT:
      return "Quit Without Saving?";

    case STRING_DESC_USB_AS_FIXED_DISKS_3:
    case STRING_DESC_WSS_PORT_3:
    case STRING_DESC_SB_PORT_3:
    case STRING_DESC_GAMEPAD_PORT_3:
    case STRING_DESC_MPU_PORT_3:
    case STRING_DESC_DIGITAL_1:
    case STRING_DESC_DIGITAL_3:
    case STRING_DESC_MAC_ADDRESS_1:
    case STRING_DESC_MAC_ADDRESS_3:
    case STRING_DESC_FAN1_TARGET_RPM_3:
    case STRING_DESC_FAN2_TARGET_RPM_3:
    case STRING_DESC_FAN3_TARGET_RPM_3:
    case STRING_DESC_BOOT_TUNE_1:
    case STRING_DESC_BOOT_TUNE_3:
    case STRING_DESC_LEDS_ENABLED_1:
    case STRING_DESC_LEDS_ENABLED_3:
    case STRING_DESC_LEDS_INTENSITY_1:
    case STRING_DESC_LEDS_INTENSITY_3:
    case STRING_DESC_CLICK_ENABLED_3:
    case STRING_DESC_QUIT_1:
    case STRING_DESC_QUIT_2:
    case STRING_DESC_QUIT_3:
      return "";
    default : return "Unknown String";
  }
}

u16 string_to_u16(const char *str)
{
  int i;
  u16 num = 0;
  for (i = 0; str[i] != '\0'; i++) {
    if (str[i] < '0' || str[i] > '9') continue;
    num = num * 10 + (str[i] - '0');
  }
  return num;
}

u8 num_digits(u16 num)
{
  u8 digits = 0;
  while (num != 0) {
    digits++;
    num /= 10;
  }
  return digits;
}

void u16_to_string(u16 num, char *str, int len)
{
  int i, rem;
  int l = 0;
  int n = num;
  int orig = num;

  while (n != 0) {
    l++;
    n /= 10;
  }
  for (i = 0; i < len; i++) {
    if (i >= l) {
      str[len - (i + 1)] = ' ';
    } else {
      rem = num % 10;
      num = num / 10;
      str[len - (i + 1)] = rem + '0';
    }
  }
  if (orig == 0) str[len - 1] = '0';
  str[len] = '\0';
}

void add_char_to_end(char *str, int len, char c)
{
  int i;
  u8 digits = num_digits(string_to_u16(str));
  if (digits == len) return;
  if (digits == 0) {
    str[len - 1] = c;
    return;
  }
  for (i = 0; i < len; i++) {
    if (i == len - 1) {
      str[i] = c;
    } else {
      str[i] = str[i+1];
    }
  }
}

void del_char_at_end(char *str, int len)
{
  int i;
  for (i = len - 1; i >= 0; i--) {
    if (i == 0) {
      str[i] = ' ';
    } else {
      str[i] = str[i-1];
    }
  }
  if (str[len - 1] == ' ') str[len - 1] = '0';
}