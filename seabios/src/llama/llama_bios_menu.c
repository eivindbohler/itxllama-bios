#include "llama/llama_bios_menu.h"
#include "llama/llama_bios_menu_handlers.h"
#include "llama/llama_bios_menu_ui.h"
#include "llama/llama_bios_settings.h"
#include "llama/llama_fonts.h"
#include "llama/llama_strings.h"
#include "bregs.h"   // struct bregs
#include "config.h"  // SEG_BIOS
#include "farptr.h"  // FLATPTR_TO_SEG, FLATPTR_TO_OFFSET, SEGOFF
#include "stacks.h"  // farcall16big
#include "string.h"  // strlen
#include "util.h"    // get_keystroke_full

#define NUM_MAIN_MENU_ENTRIES 7

static u8 ext_fm_present(llama_bios_settings *lbs); // forward declaration
static u8 leds_enabled(llama_bios_settings *lbs);   // forward declaration

int main_menu_selection = 0;
int submenu_selection = -1;
int change_selection = -1;
int quit_menu_selection = -1;

submenu_entry quit_menu_entry = {
  STRING_NAME_QUIT,
  STRING_DESC_QUIT_1,
  STRING_DESC_QUIT_2,
  STRING_DESC_QUIT_3,
  2,
  { // change entries
    { "Save & Quit" , 1 },
    { "Just Quit", 0 }
  },
  NULL,
  NULL
};

main_menu_entry main_menu_entries[NUM_MAIN_MENU_ENTRIES] = {
  {
    STRING_MAIN_CPU_SETTINGS,
    3,
    { // submenus
      {
        STRING_NAME_CPU_FREQ,
        STRING_DESC_CPU_FREQ_1,
        STRING_DESC_CPU_FREQ_2,
        STRING_DESC_CPU_FREQ_3,
        6,
        { // change entries
          { "60 MHz",  0 },
          { "100 MHz", 1 },
          { "200 MHz", 2 },
          { "300 MHz", 3 },
          { "466 MHz", 4 },
          { "500 MHz", 5 }
        },
        handler_get_cpu_freq_index,
        handler_set_cpu_freq_index,
        "",   // input unit
        NULL, // get_num_value
        NULL, // set_num_value
        NULL  // get_enabled
      },
      {
        STRING_NAME_L1_CACHE,
        STRING_DESC_L1_CACHE_1,
        STRING_DESC_L1_CACHE_2,
        STRING_DESC_L1_CACHE_3,
        2,
        { // change entries
          { "Enabled" , 1 },
          { "Disabled", 0 }
        },
        handler_get_l1_cache,
        handler_set_l1_cache,
        "",   // input unit
        NULL, // get_num_value
        NULL, // set_num_value
        NULL  // get_enabled
      },
      {
        STRING_NAME_L2_CACHE,
        STRING_DESC_L2_CACHE_1,
        STRING_DESC_L2_CACHE_2,
        STRING_DESC_L2_CACHE_3,
        2,
        { // change entries
          { "Enabled" , 1 },
          { "Disabled", 0 }
        },
        handler_get_l2_cache,
        handler_set_l2_cache,
        "",   // input unit
        NULL, // get_num_value
        NULL, // set_num_value
        NULL  // get_enabled
      }
    },
    NULL // update_loop
  },
  {
    STRING_MAIN_DISK_SETTINGS,
    2,
    { // submenus
      {
        STRING_NAME_BOOT_ORDER,
        STRING_DESC_BOOT_ORDER_1,
        STRING_DESC_BOOT_ORDER_2,
        STRING_DESC_BOOT_ORDER_3,
        6,
        { // change entries
          { "SATA, SD, USB, VFloppy", 0 },
          { "SATA, USB, SD, VFloppy", 1 },
          { "SD, SATA, USB, VFloppy", 2 },
          { "SD, USB, SATA, VFloppy", 3 },
          { "USB, SATA, SD, VFloppy", 4 },
          { "USB, SD, SATA, VFloppy", 5 }
        },
        handler_get_boot_order_index,
        handler_set_boot_order_index,
        "",   // input unit
        NULL, // get_num_value
        NULL, // set_num_value
        NULL  // get_enabled
      },
      {
        STRING_NAME_USB_AS_FIXED_DISKS,
        STRING_DESC_USB_AS_FIXED_DISKS_1,
        STRING_DESC_USB_AS_FIXED_DISKS_2,
        STRING_DESC_USB_AS_FIXED_DISKS_3,
        2,
        { // change entries
          { "Enabled" , 1 },
          { "Disabled", 0 }
        },
        handler_get_usb_as_fixed_disks,
        handler_set_usb_as_fixed_disks,
        "",   // input unit
        NULL, // get_num_value
        NULL, // set_num_value
        NULL  // get_enabled
      }
    },
    NULL // update_loop
  },
  {
    STRING_MAIN_COM_SETTINGS,
    2,
    { // submenus
      {
        STRING_NAME_COM1_BASE_CLOCK,
        STRING_DESC_COM1_BASE_CLOCK_1,
        STRING_DESC_COM1_BASE_CLOCK_2,
        STRING_DESC_COM1_BASE_CLOCK_3,
        3,
        { // change entries
          { "115.2 kHz", 0 },
          { "3 MHz"    , 1 },
          { "6 MHz"    , 2 }
        },
        handler_get_com1_base_clock_index,
        handler_set_com1_base_clock_index,
        "",   // input unit
        NULL, // get_num_value
        NULL, // set_num_value
        NULL  // get_enabled
      },
      {
        STRING_NAME_COM2_BASE_CLOCK,
        STRING_DESC_COM2_BASE_CLOCK_1,
        STRING_DESC_COM2_BASE_CLOCK_2,
        STRING_DESC_COM2_BASE_CLOCK_3,
        3,
        { // change entries
          { "115.2 kHz", 0 },
          { "3 MHz"    , 1 },
          { "6 Mhz"    , 2 }
        },
        handler_get_com2_base_clock_index,
        handler_set_com2_base_clock_index,
        "",   // input unit
        NULL, // get_num_value
        NULL, // set_num_value
        NULL  // get_enabled
      }
    },
    NULL // update_loop
  },
  {
    STRING_MAIN_AUDIO_SETTINGS,
    7,
    { // submenus
      {
        STRING_NAME_WSS_PORT,
        STRING_DESC_WSS_PORT_1,
        STRING_DESC_WSS_PORT_2,
        STRING_DESC_WSS_PORT_3,
        3,
        { // change entries
          { "0x534", 0x534 },
          { "0x540", 0x540 },
          { "0x604", 0x604 }
        },
        handler_get_cs4237_wss_port,
        handler_set_cs4237_wss_port,
        "",   // input unit
        NULL, // get_num_value
        NULL, // set_num_value
        NULL  // get_enabled
      },
      {
        STRING_NAME_SYN_PORT,
        STRING_DESC_SYN_PORT_1,
        STRING_DESC_SYN_PORT_2,
        STRING_DESC_SYN_PORT_3,
        2,
        { // change entries
          { "0x388", 0x388 },
          { "0x3F8", 0x3F8 }
        },
        handler_get_cs4237_syn_port,
        handler_set_cs4237_syn_port,
        "",   // input unit
        NULL, // get_num_value
        NULL, // set_num_value
        NULL  // get_enabled
      },
      {
        STRING_NAME_SB_PORT,
        STRING_DESC_SB_PORT_1,
        STRING_DESC_SB_PORT_2,
        STRING_DESC_SB_PORT_3,
        2,
        { // change entries
          { "0x220", 0x220 },
          { "0x240", 0x240 }
        },
        handler_get_cs4237_sb_port,
        handler_set_cs4237_sb_port,
        "",   // input unit
        NULL, // get_num_value
        NULL, // set_num_value
        NULL  // get_enabled
      },
      {
        STRING_NAME_GAMEPAD_PORT,
        STRING_DESC_GAMEPAD_PORT_1,
        STRING_DESC_GAMEPAD_PORT_2,
        STRING_DESC_GAMEPAD_PORT_3,
        2,
        { // change entries
          { "0x200", 0x200 },
          { "0x208", 0x208 }
        },
        handler_get_cs4237_gamepad_port,
        handler_set_cs4237_gamepad_port,
        "",   // input unit
        NULL, // get_num_value
        NULL, // set_num_value
        NULL  // get_enabled
      },
      {
        STRING_NAME_MPU_PORT,
        STRING_DESC_MPU_PORT_1,
        STRING_DESC_MPU_PORT_2,
        STRING_DESC_MPU_PORT_3,
        2,
        { // change entries
          { "0x330", 0x330 },
          { "0x300", 0x300 }
        },
        handler_get_cs4237_mpu_port,
        handler_set_cs4237_mpu_port,
        "",   // input unit
        NULL, // get_num_value
        NULL, // set_num_value
        NULL  // get_enabled
      },
      {
        STRING_NAME_EXT_FM,
        STRING_DESC_EXT_FM_1,
        STRING_DESC_EXT_FM_2,
        STRING_DESC_EXT_FM_3,
        2,
        { // change entries
          { "Enabled" , 1 },
          { "Disabled", 0 }
        },
        handler_get_cs4237_ext_fm,
        handler_set_cs4237_ext_fm,
        "",   // input unit
        NULL, // get_num_value
        NULL, // set_num_value
        ext_fm_present
      },
      {
        STRING_NAME_DIGITAL,
        STRING_DESC_DIGITAL_1,
        STRING_DESC_DIGITAL_2,
        STRING_DESC_DIGITAL_3,
        2,
        { // change entries
          { "Enabled" , 1 },
          { "Disabled", 0 }
        },
        handler_get_cs4237_digital,
        handler_set_cs4237_digital,
        "",   // input unit
        NULL, // get_num_value
        NULL, // set_num_value
        NULL  // get_enabled
      }
    },
    NULL // update_loop
  },
  {
    STRING_MAIN_ETHERNET_SETTINGS,
    1,
    { // submenus
      {
        STRING_NAME_MAC_ADDRESS,
        STRING_DESC_MAC_ADDRESS_1,
        STRING_DESC_MAC_ADDRESS_2,
        STRING_DESC_MAC_ADDRESS_3,
        6,
        { // change entries
          { "00:0B:B4:00:00:01", 0 },
          { "00:0B:B4:00:00:02", 1 },
          { "00:0B:B4:00:00:03", 2 },
          { "00:0B:B4:00:00:04", 3 },
          { "00:0B:B4:00:00:05", 4 },
          { "00:0B:B4:00:00:06", 5 },
        },
        handler_get_mac_address_index,
        handler_set_mac_address_index,
        "",   // input unit
        NULL, // get_num_value
        NULL, // set_num_value
        NULL  // get_enabled
      }
    },
    NULL // update_loop
  },
  {
    STRING_MAIN_FAN_CONTROL,
    3,
    { // submenus
      {
        STRING_NAME_FAN1_TARGET_RPM,
        STRING_DESC_FAN1_TARGET_RPM_1,
        STRING_DESC_FAN1_TARGET_RPM_2,
        STRING_DESC_FAN1_TARGET_RPM_3,
        0,
        {},    // change entries
        NULL,  // get_value
        NULL,  // set_value
        "RPM", // input unit
        handler_get_fan1_target_rpm,
        handler_set_fan1_target_rpm,
        NULL   // get_enabled
      },
      {
        STRING_NAME_FAN2_TARGET_RPM,
        STRING_DESC_FAN2_TARGET_RPM_1,
        STRING_DESC_FAN2_TARGET_RPM_2,
        STRING_DESC_FAN2_TARGET_RPM_3,
        0,
        {},    // change entries
        NULL,  // get_value
        NULL,  // set_value
        "RPM", // input unit
        handler_get_fan2_target_rpm,
        handler_set_fan2_target_rpm,
        NULL   // get_enabled
      },
      {
        STRING_NAME_FAN3_TARGET_RPM,
        STRING_DESC_FAN3_TARGET_RPM_1,
        STRING_DESC_FAN3_TARGET_RPM_2,
        STRING_DESC_FAN3_TARGET_RPM_3,
        0,
        {},    // change entries
        NULL,  // get_value
        NULL,  // set_value
        "RPM", // input unit
        handler_get_fan3_target_rpm,
        handler_set_fan3_target_rpm,
        NULL   // get_enabled
      }
    },
    draw_fan_rpms
  },
  {
    STRING_MAIN_MISC_SETTINGS,
    4,
    { // submenus
      {
        STRING_NAME_BOOT_TUNE,
        STRING_DESC_BOOT_TUNE_1,
        STRING_DESC_BOOT_TUNE_2,
        STRING_DESC_BOOT_TUNE_3,
        3,
        { // change entries
          { "None",     0 },
          { "Ducks",    1 },
          { "Mushroom", 2 }
        },
        handler_get_boot_tune_index,
        handler_set_boot_tune_index,
        "",   // input unit
        NULL, // get_num_value
        NULL, // set_num_value
        NULL  // get_enabled
      },
      {
        STRING_NAME_LEDS_ENABLED,
        STRING_DESC_LEDS_ENABLED_1,
        STRING_DESC_LEDS_ENABLED_2,
        STRING_DESC_LEDS_ENABLED_3,
        2,
        { // change entries
          { "Enabled" , 1 },
          { "Disabled", 0 }
        },
        handler_get_leds_enabled,
        handler_set_leds_enabled,
        "",   // input unit
        NULL, // get_num_value
        NULL, // set_num_value
        NULL  // get_enabled
      },
      {
        STRING_NAME_LEDS_INTENSITY,
        STRING_DESC_LEDS_INTENSITY_1,
        STRING_DESC_LEDS_INTENSITY_2,
        STRING_DESC_LEDS_INTENSITY_3,
        6,
        { // change entries
          { "5%",   13 },
          { "10%",  26 },
          { "25%",  64 },
          { "50%",  128 },
          { "75%",  191 },
          { "100%", 254 }
        },
        handler_get_leds_intensity,
        handler_set_leds_intensity,
        "",   // input unit
        NULL, // get_num_value
        NULL, // set_num_value
        leds_enabled
      },
      {
        STRING_NAME_CLICK_ENABLED,
        STRING_DESC_CLICK_ENABLED_1,
        STRING_DESC_CLICK_ENABLED_2,
        STRING_DESC_CLICK_ENABLED_3,
        2,
        { // change entries
          { "Enabled" , 1 },
          { "Disabled", 0 }
        },
        handler_get_click_enabled,
        handler_set_click_enabled,
        "",   // input unit
        NULL, // get_num_value
        NULL, // set_num_value
        NULL  // get_enabled
      }
    },
    NULL // update_loop
  }
};

static u8 ext_fm_present(llama_bios_settings *lbs)
{
  return lbs->cs4237_ext_fm_present;
}

static u8 leds_enabled(llama_bios_settings *lbs)
{
  return handler_get_leds_enabled(lbs);
}

static void reboot(void)
{
    struct bregs br;
    memset(&br, 0, sizeof(br));
    br.code = SEGOFF(SEG_BIOS, (u32)reset_vector);
    farcall16big(&br);
}

static void save_and_reboot(llama_bios_settings *lbs)
{
  llama_save_bios_settings(lbs);
  reboot();
}

typedef enum {
  KEYBOARD_ACTION_UP,
  KEYBOARD_ACTION_DOWN,
  KEYBOARD_ACTION_LEFT,
  KEYBOARD_ACTION_RIGHT,
  KEYBOARD_ACTION_TAB,
  KEYBOARD_ACTION_SHIFT_TAB,
  KEYBOARD_ACTION_ENTER,
  KEYBOARD_ACTION_ESC,
  KEYBOARD_ACTION_F10
} keyboard_action;

static int get_scancode(void)
{
  for (;;) {
    int scancode = get_keystroke_full(500);
    if (scancode == -1) continue;
    return scancode >> 8;
  }
}

static keyboard_action get_input(void (*callback)(u8 param), u8 param)
{
  for (;;) {
    if (callback != NULL) callback(param);
    int scancode = get_keystroke_full(500);
    if (scancode == -1) continue;
    switch (scancode >> 8) {
      case 0x48: return KEYBOARD_ACTION_UP;
      case 0x50: return KEYBOARD_ACTION_DOWN;
      case 0x4B: return KEYBOARD_ACTION_LEFT;
      case 0x4D: return KEYBOARD_ACTION_RIGHT;
      case 0x0F:
        return ((scancode & 0x0F) == 0x09) ? KEYBOARD_ACTION_TAB : KEYBOARD_ACTION_SHIFT_TAB;
      case 0x1C:
      case 0xE0: return KEYBOARD_ACTION_ENTER;
      case 0x01: return KEYBOARD_ACTION_ESC;
      case 0x44: return KEYBOARD_ACTION_F10;
      default: break;
    }
  }
}

static int menu_forward(int num_entries, int selection)
{
  selection++;
  if (selection == num_entries) selection = 0;
  return selection;
}

static int menu_backward(int num_entries, int selection)
{
  selection--;
  if (selection < 0) selection = num_entries - 1;
  return selection;
}

static int get_index(
  change_entry *entries,
  int num_entries,
  u16 value
) {
  int i;
  for (i = 0; i < num_entries; i++) {
    if (entries[i].value == value) return i;
  }
  return 0;
}

static void change_loop(llama_bios_settings *lbs)
{
  main_menu_entry menu_entry = main_menu_entries[main_menu_selection];
  submenu_entry submenu_entry = menu_entry.submenus[submenu_selection];
  u16 value = submenu_entry.get_selection_value(lbs);
  change_selection = get_index(
    submenu_entry.change_entries,
    submenu_entry.num_change_entries,
    value
  );

  for (;;) {
    draw_menus(
      lbs,
      main_menu_entries,
      quit_menu_entry,
      NUM_MAIN_MENU_ENTRIES,
      main_menu_selection,
      submenu_selection,
      change_selection,
      quit_menu_selection,
      NULL
    );

    change_entry change_entry = submenu_entry.change_entries[change_selection];

    // wait for keypress
    switch (get_input(NULL, 0)) {
      case KEYBOARD_ACTION_UP:
      case KEYBOARD_ACTION_SHIFT_TAB:
        change_selection = menu_backward(submenu_entry.num_change_entries, change_selection);
        break;
      case KEYBOARD_ACTION_DOWN:
      case KEYBOARD_ACTION_TAB:
        change_selection = menu_forward(submenu_entry.num_change_entries, change_selection);
        break;
      case KEYBOARD_ACTION_LEFT:
      case KEYBOARD_ACTION_RIGHT:
      case KEYBOARD_ACTION_F10: break;
      case KEYBOARD_ACTION_ENTER:
        submenu_entry.set_selection_value(lbs, change_entry.value);
      case KEYBOARD_ACTION_ESC:
        change_selection = -1;
        return;
    }
  }
}

static void input_loop(submenu_entry submenu_entry, llama_bios_settings *lbs)
{
  u16 input_max_length = 4;
  u16 input_value = submenu_entry.get_input_value(lbs);
  char input[5];
  u16_to_string(input_value, input, input_max_length);

  for (;;) {
    draw_menus(
      lbs,
      main_menu_entries,
      quit_menu_entry,
      NUM_MAIN_MENU_ENTRIES,
      main_menu_selection,
      submenu_selection,
      change_selection,
      quit_menu_selection,
      input
    );

    // wait for keypress
    int scancode = get_scancode();
    switch (scancode) {
      case 0x02 ... 0x0B: {
        const char ch = "1234567890"[scancode - 0x02];
        add_char_to_end(input, input_max_length, ch);
        break;
      }
      case 0x0E: { // BACKSPACE
        del_char_at_end(input, input_max_length);
        break;
      }
      case 0x1C:
      case 0xE0: // ENTER
        input_value = string_to_u16(input);
        submenu_entry.set_input_value(lbs, input_value);
      case 0x01: // ESC
        return;
      default:
        break;
    }
  }
}

static void quit_menu_loop(llama_bios_settings *lbs)
{
  quit_menu_selection = 0;

  for (;;) {
    draw_menus(
      lbs,
      main_menu_entries,
      quit_menu_entry,
      NUM_MAIN_MENU_ENTRIES,
      main_menu_selection,
      submenu_selection,
      change_selection,
      quit_menu_selection,
      NULL
    );

    // wait for keypress
    switch (get_input(NULL, 0)) {
      case KEYBOARD_ACTION_UP:
      case KEYBOARD_ACTION_SHIFT_TAB:
        quit_menu_selection = menu_backward(quit_menu_entry.num_change_entries, quit_menu_selection);
        break;
      case KEYBOARD_ACTION_DOWN:
      case KEYBOARD_ACTION_TAB:
        quit_menu_selection = menu_forward(quit_menu_entry.num_change_entries, quit_menu_selection);
        break;
      case KEYBOARD_ACTION_LEFT:
      case KEYBOARD_ACTION_RIGHT:
      case KEYBOARD_ACTION_F10: break;
      case KEYBOARD_ACTION_ENTER:
        if (quit_menu_selection == 0) save_and_reboot(lbs);
        reboot();
      case KEYBOARD_ACTION_ESC:
        quit_menu_selection = -1;
        return;
    }
  }
}

static void submenu_loop(llama_bios_settings *lbs)
{
  int need_redraw = 0;
  submenu_selection = 0;

  for (;;) {
    if (need_redraw) {
      draw_background();
      need_redraw = 0;
    }
    draw_menus(
      lbs,
      main_menu_entries,
      quit_menu_entry,
      NUM_MAIN_MENU_ENTRIES,
      main_menu_selection,
      submenu_selection,
      change_selection,
      quit_menu_selection,
      NULL
    );

    main_menu_entry menu_entry = main_menu_entries[main_menu_selection];

    // wait for keypress
    switch (get_input(menu_entry.update_loop, 0)) {
      case KEYBOARD_ACTION_UP:
      case KEYBOARD_ACTION_SHIFT_TAB: {
        int orig_selection = submenu_selection;
        submenu_selection = menu_backward(menu_entry.num_submenus, submenu_selection);
        for (;;) {
          submenu_entry submenu_entry = menu_entry.submenus[submenu_selection];
          if (submenu_selection == orig_selection) break;
          if (submenu_entry.get_enabled != NULL && !submenu_entry.get_enabled(lbs)) {
            submenu_selection = menu_backward(menu_entry.num_submenus, submenu_selection);
          } else {
            break;
          }
        }
        break;
      }
      case KEYBOARD_ACTION_DOWN:
      case KEYBOARD_ACTION_TAB: {
        int orig_selection = submenu_selection;
        submenu_selection = menu_forward(menu_entry.num_submenus, submenu_selection);
        for (;;) {
          submenu_entry submenu_entry = menu_entry.submenus[submenu_selection];
          if (submenu_selection == orig_selection) break;
          if (submenu_entry.get_enabled != NULL && !submenu_entry.get_enabled(lbs)) {
            submenu_selection = menu_forward(menu_entry.num_submenus, submenu_selection);
          } else {
            break;
          }
        }
        break;
      }
      case KEYBOARD_ACTION_RIGHT: break;
      case KEYBOARD_ACTION_ENTER: {
        submenu_entry submenu_entry = menu_entry.submenus[submenu_selection];
        if (submenu_entry.get_enabled != NULL && !submenu_entry.get_enabled(lbs)) break;
        if (submenu_entry.get_input_value != NULL) {
          input_loop(submenu_entry, lbs);
        } else {
          change_loop(lbs);
        }
        need_redraw = 1;
        break;
      }
      case KEYBOARD_ACTION_LEFT:
        submenu_selection = -1;
        return;
      case KEYBOARD_ACTION_ESC:
        if (lbs->has_changes) {
          quit_menu_loop(lbs);
          draw_background();
        } else {
          reboot();
        }
        break;
      case KEYBOARD_ACTION_F10:
        save_and_reboot(lbs);
        break;
    }
  }
}

static void main_menu_loop(llama_bios_settings *lbs)
{
  draw_background();

  for (;;) {
    clear_area(6, 43, 36, 11); // clear details area
    clear_area(21, 1, 78, 3);  // clear bottom area
    draw_menus(
      lbs,
      main_menu_entries,
      quit_menu_entry,
      NUM_MAIN_MENU_ENTRIES,
      main_menu_selection,
      submenu_selection,
      change_selection,
      quit_menu_selection,
      NULL
    );

    void (*callback)(u8 param) = main_menu_entries[main_menu_selection].update_loop;

    // wait for keypress
    switch (get_input(callback, 1)) {
      case KEYBOARD_ACTION_UP:
      case KEYBOARD_ACTION_SHIFT_TAB:
        main_menu_selection = menu_backward(NUM_MAIN_MENU_ENTRIES, main_menu_selection);
        break;
      case KEYBOARD_ACTION_DOWN:
      case KEYBOARD_ACTION_TAB:
        main_menu_selection = menu_forward(NUM_MAIN_MENU_ENTRIES, main_menu_selection);
        break;
      case KEYBOARD_ACTION_LEFT:
      case KEYBOARD_ACTION_ENTER: break;
      case KEYBOARD_ACTION_RIGHT:
        submenu_loop(lbs);
        break;
      case KEYBOARD_ACTION_ESC:
        if (lbs->has_changes) {
          quit_menu_loop(lbs);
          draw_background();
        } else {
          reboot();
        }
        break;
      case KEYBOARD_ACTION_F10:
        save_and_reboot(lbs);
        break;
    }
  }
}

void llama_bios_menu(llama_bios_settings *lbs)
{
  hide_cursor();

  load_custom_fonts(llama_fonts+LLAMA_FONT_0_ARRAY_POS, LLAMA_FONT_0, 1);
  load_custom_fonts(llama_fonts+LLAMA_FONT_1_ARRAY_POS, LLAMA_FONT_1, 7);
  load_custom_fonts(llama_fonts+LLAMA_FONT_UP_ARROW_ARRAY_POS, LLAMA_FONT_UP_ARROW, 4);

  main_menu_loop(lbs);
}