#ifndef __LLAMA_STRINGS_H
#define __LLAMA_STRINGS_H

#include "types.h"

#define STRING_MAIN_CPU_SETTINGS          101

#define STRING_NAME_CPU_FREQ              111
#define STRING_DESC_CPU_FREQ_1            112
#define STRING_DESC_CPU_FREQ_2            113
#define STRING_DESC_CPU_FREQ_3            114

#define STRING_NAME_L1_CACHE              121
#define STRING_DESC_L1_CACHE_1            122
#define STRING_DESC_L1_CACHE_2            123
#define STRING_DESC_L1_CACHE_3            124

#define STRING_NAME_L2_CACHE              131
#define STRING_DESC_L2_CACHE_1            132
#define STRING_DESC_L2_CACHE_2            133
#define STRING_DESC_L2_CACHE_3            134

#define STRING_MAIN_DISK_SETTINGS         201

#define STRING_NAME_BOOT_ORDER            211
#define STRING_DESC_BOOT_ORDER_1          212
#define STRING_DESC_BOOT_ORDER_2          213
#define STRING_DESC_BOOT_ORDER_3          214

#define STRING_NAME_USB_AS_FIXED_DISKS    221
#define STRING_DESC_USB_AS_FIXED_DISKS_1  222
#define STRING_DESC_USB_AS_FIXED_DISKS_2  223
#define STRING_DESC_USB_AS_FIXED_DISKS_3  224

#define STRING_MAIN_COM_SETTINGS          301

#define STRING_NAME_COM1_BASE_CLOCK       311
#define STRING_DESC_COM1_BASE_CLOCK_1     312
#define STRING_DESC_COM1_BASE_CLOCK_2     313
#define STRING_DESC_COM1_BASE_CLOCK_3     314

#define STRING_NAME_COM2_BASE_CLOCK       321
#define STRING_DESC_COM2_BASE_CLOCK_1     322
#define STRING_DESC_COM2_BASE_CLOCK_2     323
#define STRING_DESC_COM2_BASE_CLOCK_3     324

#define STRING_MAIN_AUDIO_SETTINGS        401

#define STRING_NAME_WSS_PORT              411
#define STRING_DESC_WSS_PORT_1            412
#define STRING_DESC_WSS_PORT_2            413
#define STRING_DESC_WSS_PORT_3            414

#define STRING_NAME_SYN_PORT              421
#define STRING_DESC_SYN_PORT_1            422
#define STRING_DESC_SYN_PORT_2            423
#define STRING_DESC_SYN_PORT_3            424

#define STRING_NAME_SB_PORT               431
#define STRING_DESC_SB_PORT_1             432
#define STRING_DESC_SB_PORT_2             433
#define STRING_DESC_SB_PORT_3             434

#define STRING_NAME_GAMEPAD_PORT          441
#define STRING_DESC_GAMEPAD_PORT_1        442
#define STRING_DESC_GAMEPAD_PORT_2        443
#define STRING_DESC_GAMEPAD_PORT_3        444

#define STRING_NAME_MPU_PORT              451
#define STRING_DESC_MPU_PORT_1            452
#define STRING_DESC_MPU_PORT_2            453
#define STRING_DESC_MPU_PORT_3            454

#define STRING_NAME_EXT_FM                461
#define STRING_DESC_EXT_FM_1              462
#define STRING_DESC_EXT_FM_2              463
#define STRING_DESC_EXT_FM_3              464

#define STRING_NAME_DIGITAL               471
#define STRING_DESC_DIGITAL_1             472
#define STRING_DESC_DIGITAL_2             473
#define STRING_DESC_DIGITAL_3             474

#define STRING_MAIN_ETHERNET_SETTINGS     501

#define STRING_NAME_MAC_ADDRESS           511
#define STRING_DESC_MAC_ADDRESS_1         512
#define STRING_DESC_MAC_ADDRESS_2         513
#define STRING_DESC_MAC_ADDRESS_3         514

#define STRING_MAIN_FAN_CONTROL           601

#define STRING_NAME_FAN1_TARGET_RPM       611
#define STRING_DESC_FAN1_TARGET_RPM_1     612
#define STRING_DESC_FAN1_TARGET_RPM_2     613
#define STRING_DESC_FAN1_TARGET_RPM_3     614

#define STRING_NAME_FAN2_TARGET_RPM       621
#define STRING_DESC_FAN2_TARGET_RPM_1     622
#define STRING_DESC_FAN2_TARGET_RPM_2     623
#define STRING_DESC_FAN2_TARGET_RPM_3     624

#define STRING_NAME_FAN3_TARGET_RPM       631
#define STRING_DESC_FAN3_TARGET_RPM_1     632
#define STRING_DESC_FAN3_TARGET_RPM_2     633
#define STRING_DESC_FAN3_TARGET_RPM_3     634

#define STRING_MAIN_MISC_SETTINGS         701

#define STRING_NAME_BOOT_TUNE             711
#define STRING_DESC_BOOT_TUNE_1           712
#define STRING_DESC_BOOT_TUNE_2           713
#define STRING_DESC_BOOT_TUNE_3           714

#define STRING_NAME_LEDS_ENABLED          721
#define STRING_DESC_LEDS_ENABLED_1        722
#define STRING_DESC_LEDS_ENABLED_2        723
#define STRING_DESC_LEDS_ENABLED_3        724

#define STRING_NAME_LEDS_INTENSITY        731
#define STRING_DESC_LEDS_INTENSITY_1      732
#define STRING_DESC_LEDS_INTENSITY_2      733
#define STRING_DESC_LEDS_INTENSITY_3      734

#define STRING_NAME_CLICK_ENABLED         741
#define STRING_DESC_CLICK_ENABLED_1       742
#define STRING_DESC_CLICK_ENABLED_2       743
#define STRING_DESC_CLICK_ENABLED_3       744

#define STRING_NAME_QUIT                  990
#define STRING_DESC_QUIT_1                991
#define STRING_DESC_QUIT_2                992
#define STRING_DESC_QUIT_3                993

const char *get_string(u16 id);
u16 string_to_u16(const char *str);
u8 num_digits(u16 num);
void u16_to_string(u16 num, char *str, int len);
void add_char_to_end(char *str, int len, char c);
void del_char_at_end(char *str, int len);

#endif // __LLAMA_STRINGS_H