#ifndef __LLAMA_BIOS_MENU_UI_H
#define __LLAMA_BIOS_MENU_UI_H

#include "llama/llama_bios_menu.h"
#include "llama/llama_bios_settings.h"

void hide_cursor(void);
void draw_background(void);
void clear_area(u8 row, u8 col, u8 width, u8 height);
void draw_menus(
  llama_bios_settings *lbs,
  main_menu_entry *main_menu_entries,
  submenu_entry quit_menu_entry,
  int num_main_entries,
  int selected_main_entry,
  int selected_sub_entry,
  int selected_change_entry,
  int selected_quit_entry,
  const char *input
);
void draw_fan_rpms(u8 reset);

#endif // __LLAMA_BIOS_MENU_UI_H