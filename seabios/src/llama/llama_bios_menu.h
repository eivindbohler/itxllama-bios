#ifndef __LLAMA_BIOS_MENU_H
#define __LLAMA_BIOS_MENU_H

#include "llama/llama_bios_settings.h"
#include "llama/llama_bios_menu.h"

typedef struct {
  const char name[30];
  u16 value;
} change_entry;

typedef struct {
  u16 name_id;
  u16 description1_id;
  u16 description2_id;
  u16 description3_id;
  int num_change_entries;
  change_entry change_entries[10];
  u16 (*get_selection_value)(llama_bios_settings *lbs);
  void (*set_selection_value)(llama_bios_settings *lbs, u16 value);
  const char input_unit[10];
  u16 (*get_input_value)(llama_bios_settings *lbs);
  void (*set_input_value)(llama_bios_settings *lbs, u16 value);
  u8 (*get_enabled)(llama_bios_settings *lbs);
} submenu_entry;

typedef struct {
  u16 name_id;
  int num_submenus;
  submenu_entry submenus[10];
  void (*update_loop)(u8 param);
} main_menu_entry;

void llama_bios_menu(llama_bios_settings *lbs);

#endif // __LLAMA_BIOS_MENU_H