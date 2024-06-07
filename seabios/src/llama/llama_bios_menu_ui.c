#include "llama/llama_bios_menu_ui.h"
#include "llama/llama_fonts.h"
#include "llama/llama_strings.h"
#include "llama/llama_emc2303.h"
#include "bregs.h"  // struct bregs
#include "farptr.h" // FLATPTR_TO_SEG, FLATPTR_TO_OFFSET
#include "stacks.h" // call16_int
#include "string.h" // memset, strlen
#include "output.h" // snprintf
#include "util.h"   // timer_calc, timer_check

#define BLACK 0x00
#define BLUE 0x01
#define GREEN 0x02
#define CYAN 0x03
#define RED 0x04
#define MAGENTA 0x05
#define BROWN 0x06
#define LIGHT_GRAY 0x07
#define DARK_GRAY 0x08
#define LIGHT_BLUE 0x09
#define LIGHT_GREEN 0x0A
#define LIGHT_CYAN 0x0B
#define LIGHT_RED 0x0C
#define LIGHT_MAGENTA 0x0D
#define YELLOW 0x0E
#define WHITE 0x0F

#define COLOR(foreground, background) \
  ((foreground & 0x0F) | ((background & 0x0F) << 4))

#define BACKGROUND BLUE
#define ACTIVE_BACKGROUND RED
#define PASSIVE_BACKGROUND LIGHT_GRAY
#define PASSIVE_TEXT WHITE
#define ACTIVE_TEXT YELLOW


static change_entry get_change_entry(
  change_entry *change_entries,
  int num_change_entries,
  u16 value
) {
  int i;
  if (num_change_entries < 1) {
    return (change_entry){
      .name = "ERROR",
      .value = 0
    };
  }
  for (i = 0; i < num_change_entries; i++) {
    if (change_entries[i].value == value) return change_entries[i];
  }
  return change_entries[0];
}

static void set_cursor_position(u8 row, u8 col)
{
  struct bregs br;
  memset(&br, 0, sizeof(br));
  br.flags = F_IF;
  br.ah = 0x02;
  br.dh = row;
  br.dl = col;
  call16_int(0x10, &br);
}

// color: high 4 bits = background, low 4 bits = foreground
static void print_color_char(const char c, u8 color, u16 repeat)
{
  struct bregs br;
  memset(&br, 0, sizeof(br));
  br.flags = F_IF;
  br.ah = 0x09;
  br.al = c;
  br.bl = color;
  br.cx = repeat;
  call16_int(0x10, &br);
}

// color: high 4 bits = background, low 4 bits = foreground
static void print_color_string(const char *str, u8 color, u8 row, u8 col)
{
  char string[80];
  snprintf(string, 80, "%s", str);
  struct bregs br;
  memset(&br, 0, sizeof(br));
  br.flags = F_IF;
  br.ah = 0x13;
  br.al = 0;  // subservice 0
  br.bl = color;
  br.cx = strlen(string);
  br.dh = row;
  br.dl = col;
  br.es = FLATPTR_TO_SEG(string);
  br.bp = FLATPTR_TO_OFFSET(string);
  call16_int(0x10, &br);
}

static void print_centered_color_string(const char *str, u8 color, u8 row)
{
  u8 length = strlen(str);
  print_color_string(str, color, row, (80 - length) / 2);
}

static void print_name_and_value(const char* name, const char *value, u8 color, u8 row, u8 col)
{
  char string[40];
  snprintf(string, 40, "%s: %s", name, value);
  print_color_string(string, color, row, col);
}

// clear screen = scroll up window
// color: high 4 bits = background, low 4 bits = foreground
static void clear_screen(u8 color)
{
  struct bregs br;
  memset(&br, 0, sizeof(br));
  br.flags = F_IF;
  br.ah = 0x06;
  br.al = 0;  // clear entire window
  br.bh = color;
  br.ch = 0;   // row of top left corner of window
  br.cl = 0;   // column of top left corner of window
  br.dh = 24;  // row of bottom right corner of window
  br.dl = 79;  // column of bottom right corner of window
  call16_int(0x10, &br);
}

static void draw_bottom_text(submenu_entry submenu_entry) {
  u8 color = COLOR(PASSIVE_TEXT, BACKGROUND);
  clear_area(21, 1, 78, 3);
  const char *string = get_string(submenu_entry.description1_id);
  print_color_string(
    string,
    color,
    21,
    (80 - strlen(string)) / 2
  );
  string = get_string(submenu_entry.description2_id);
  print_color_string(
    string,
    color,
    22,
    (80 - strlen(string)) / 2
  );
  string = get_string(submenu_entry.description3_id);
  print_color_string(
    string,
    color,
    23,
    (80 - strlen(string)) / 2
  );
}

void hide_cursor(void)
{
  struct bregs br;
  memset(&br, 0, sizeof(br));
  br.flags = F_IF;
  br.ah = 0x01;
  br.ch = 0x26;
  br.cl = 0x07;
  call16_int(0x10, &br);
}

void draw_background(void)
{
  int row;
  u8 passive_color = COLOR(PASSIVE_TEXT, BACKGROUND);
  u8 active_color = COLOR(ACTIVE_TEXT, BACKGROUND);
  clear_screen(passive_color);

  for (row = 0; row < 25; row++) {
    set_cursor_position(row, 0);
    switch (row) {
      case 0:
        print_color_char(DOUBLE_LINES_TOP_LEFT_CORNER, passive_color, 1);
        set_cursor_position(row, 1);
        print_color_char(DOUBLE_LINES_HORIZONTAL, passive_color, 78);
        set_cursor_position(row, 79);
        print_color_char(DOUBLE_LINES_TOP_RIGHT_CORNER, passive_color, 1);
        break;
      case 1: case 2: case 18: case 19: case 21: case 22: case 23:
        print_color_char(DOUBLE_LINES_VERTICAL, passive_color, 1);
        set_cursor_position(row, 79);
        print_color_char(DOUBLE_LINES_VERTICAL, passive_color, 1);
        break;
      case 3:
        print_color_char(DOUBLE_LINES_VERTICAL_DL_RIGHT, passive_color, 1);
        set_cursor_position(row, 1);
        print_color_char(DOUBLE_LINES_HORIZONTAL, passive_color, 38);
        set_cursor_position(row, 39);
        print_color_char(DOUBLE_LINES_HORIZONTAL_SL_DOWN, passive_color, 1);
        set_cursor_position(row, 40);
        print_color_char(DOUBLE_LINES_HORIZONTAL, passive_color, 39);
        set_cursor_position(row, 79);
        print_color_char(DOUBLE_LINES_VERTICAL_DL_LEFT, passive_color, 1);
        break;
      case 4: case 6 ... 16:
        print_color_char(DOUBLE_LINES_VERTICAL, passive_color, 1);
        set_cursor_position(row, 39);
        print_color_char(SINGLE_LINE_VERTICAL, passive_color, 1);
        set_cursor_position(row, 79);
        print_color_char(DOUBLE_LINES_VERTICAL, passive_color, 1);
        break;
      case 5:
        print_color_char(DOUBLE_LINES_VERTICAL_SL_RIGHT, passive_color, 1);
        set_cursor_position(row, 1);
        print_color_char(SINGLE_LINE_HORIZONTAL, passive_color, 38);
        set_cursor_position(row, 39);
        print_color_char(SINGLE_LINE_CROSS, passive_color, 1);
        set_cursor_position(row, 40);
        print_color_char(SINGLE_LINE_HORIZONTAL, passive_color, 39);
        set_cursor_position(row, 79);
        print_color_char(DOUBLE_LINES_VERTICAL_SL_LEFT, passive_color, 1);
        break;
      case 17:
        print_color_char(DOUBLE_LINES_VERTICAL_SL_RIGHT, passive_color, 1);
        set_cursor_position(row, 1);
        print_color_char(SINGLE_LINE_HORIZONTAL, passive_color, 38);
        set_cursor_position(row, 39);
        print_color_char(SINGLE_LINE_HORIZONTAL_SL_UP, passive_color, 1);
        set_cursor_position(row, 40);
        print_color_char(SINGLE_LINE_HORIZONTAL, passive_color, 39);
        set_cursor_position(row, 79);
        print_color_char(DOUBLE_LINES_VERTICAL_SL_LEFT, passive_color, 1);
        break;
      case 20:
        print_color_char(DOUBLE_LINES_VERTICAL_SL_RIGHT, passive_color, 1);
        set_cursor_position(row, 1);
        print_color_char(SINGLE_LINE_HORIZONTAL, passive_color, 78);
        set_cursor_position(row, 79);
        print_color_char(DOUBLE_LINES_VERTICAL_SL_LEFT, passive_color, 1);
        break;
      case 24:
        print_color_char(DOUBLE_LINES_BOTTOM_LEFT_CORNER, passive_color, 1);
        set_cursor_position(row, 1);
        print_color_char(DOUBLE_LINES_HORIZONTAL, passive_color, 78);
        set_cursor_position(row, 79);
        print_color_char(DOUBLE_LINES_BOTTOM_RIGHT_CORNER, passive_color, 1);
        break;
      default:
        break;
    }
  }

  print_color_string("MAIN MENU", passive_color, 4, 4);
  print_color_string("DETAILS", passive_color, 4, 43);

  print_centered_color_string("ITX-Llama BIOS Setup", active_color, 1);
  print_centered_color_string("(C) 2023 Eivind Bohler", passive_color, 2);

  set_cursor_position(1, 74);
  print_color_char(LLAMA_FONT_0, passive_color, 1);
  set_cursor_position(1, 75);
  print_color_char(LLAMA_FONT_1, passive_color, 1);
  set_cursor_position(1, 76);
  print_color_char(LLAMA_FONT_1+1, passive_color, 1);
  set_cursor_position(1, 77);
  print_color_char(LLAMA_FONT_1+2, passive_color, 1);
  set_cursor_position(2, 74);
  print_color_char(LLAMA_FONT_1+3, passive_color, 1);
  set_cursor_position(2, 75);
  print_color_char(LLAMA_FONT_1+4, passive_color, 1);
  set_cursor_position(2, 76);
  print_color_char(LLAMA_FONT_1+5, passive_color, 1);
  set_cursor_position(2, 77);
  print_color_char(LLAMA_FONT_1+6, passive_color, 1);

  print_color_string("ESC : Quit", passive_color, 18, 4);
  char move_str[29];
  snprintf(move_str, 29, "%c %c %c %c : Move Between Items",
    LLAMA_FONT_UP_ARROW, LLAMA_FONT_DOWN_ARROW, LLAMA_FONT_LEFT_ARROW, LLAMA_FONT_RIGHT_ARROW);
  print_color_string(move_str, passive_color, 18, 43);
  print_color_string("F10 : Save & Quit", passive_color, 19, 4);
  print_color_string("ENTER   : Select Item", passive_color, 19, 43);
}

void clear_area(u8 row, u8 col, u8 width, u8 height)
{
  int i;
  for (i = row; i < row + height; i++) {
    set_cursor_position(i, col);
    print_color_char(' ', COLOR(PASSIVE_TEXT, BACKGROUND), width);
  }
}

static void draw_selection_popup(submenu_entry submenu_entry, int selected_change_entry) {
  int i;
  const char *title = get_string(submenu_entry.name_id);
  u16 title_length = strlen(title);
  u16 max_length = title_length;
  for (i = 0; i < submenu_entry.num_change_entries; i++) {
    u16 length = strlen(submenu_entry.change_entries[i].name);
    if (length > max_length) {
      max_length = length;
    }
  }
  u16 popup_width = max_length + 6;
  if (popup_width < 23) popup_width = 23;
  u16 popup_height = submenu_entry.num_change_entries + 2;
  u16 popup_row = (25 - popup_height) / 2;
  u16 popup_col = (80 - popup_width) / 2;

  u16 title_left_padding = (popup_width - title_length) / 2;
  u16 title_right_padding = popup_width - title_left_padding - title_length;

  u8 color = COLOR(BACKGROUND, PASSIVE_BACKGROUND);

  set_cursor_position(popup_row, popup_col);
  print_color_char(SINGLE_LINE_TOP_LEFT_CORNER, color, 1);
  set_cursor_position(popup_row, popup_col + 1);
  print_color_char(SINGLE_LINE_HORIZONTAL, color, title_left_padding - 2);
  set_cursor_position(popup_row, popup_col + title_left_padding - 1);
  print_color_char(' ', color, 1);
  print_color_string(title, color, popup_row, popup_col + title_left_padding);
  set_cursor_position(popup_row, popup_col + title_left_padding + title_length);
  print_color_char(' ', color, 1);
  set_cursor_position(popup_row, popup_col + title_left_padding + title_length + 1);
  print_color_char(SINGLE_LINE_HORIZONTAL, color, title_right_padding - 2);
  set_cursor_position(popup_row, popup_col + popup_width - 1);
  print_color_char(SINGLE_LINE_TOP_RIGHT_CORNER, color, 1);

  for (i = 0; i < submenu_entry.num_change_entries; i++) {
    set_cursor_position(popup_row + 1 + i, popup_col);
    print_color_char(SINGLE_LINE_VERTICAL, color, 1);
    set_cursor_position(popup_row + 1 + i, popup_col + 1);
    print_color_char(' ', color, popup_width - 2);
    set_cursor_position(popup_row + 1 + i, popup_col + popup_width - 1);
    print_color_char(SINGLE_LINE_VERTICAL, color, 1);
    set_cursor_position(popup_row + 1 + i, popup_col + popup_width);
    print_color_char(BLOCK_FULL, COLOR(BLACK, BLACK), 2);

    u8 value_color = i == selected_change_entry ? COLOR(PASSIVE_TEXT, BACKGROUND) : color;
    print_color_string(submenu_entry.change_entries[i].name, value_color, popup_row + 1 + i, popup_col + 3);
  }

  set_cursor_position(popup_row + 1 + submenu_entry.num_change_entries, popup_col);
  print_color_char(SINGLE_LINE_BOTTOM_LEFT_CORNER, color, 1);
  set_cursor_position(popup_row + 1 + submenu_entry.num_change_entries, popup_col + 1);
  print_color_char(SINGLE_LINE_HORIZONTAL, color, popup_width - 2);
  set_cursor_position(popup_row + 1 + submenu_entry.num_change_entries, popup_col + popup_width - 1);
  print_color_char(SINGLE_LINE_BOTTOM_RIGHT_CORNER, color, 1);
  set_cursor_position(popup_row + 1 + submenu_entry.num_change_entries, popup_col + popup_width);
  print_color_char(BLOCK_FULL, COLOR(BLACK, BLACK), 2);

  set_cursor_position(popup_row + 2 + submenu_entry.num_change_entries, popup_col + 2);
  print_color_char(BLOCK_FULL, COLOR(BLACK, BLACK), popup_width);
}

void draw_input_popup(
  const char *title,
  const char *unit_title,
  const char *input
) {
  int i;
  u16 title_length = strlen(title);
  u16 unit_title_length = strlen(unit_title);
  u16 input_length = strlen(input);
  u16 max_length = title_length;
  if (input_length + 1 + unit_title_length > max_length) {
    max_length = input_length + 1 + unit_title_length;
  }
  u16 popup_width = max_length + 6;
  if (popup_width < 23) popup_width = 23;
  u16 popup_height = 5;
  u16 popup_row = (25 - popup_height) / 2;
  u16 popup_col = (80 - popup_width) / 2;

  u16 title_left_padding = (popup_width - title_length) / 2;
  u16 title_right_padding = popup_width - title_left_padding - title_length;

  u8 color = COLOR(BACKGROUND, PASSIVE_BACKGROUND);

  set_cursor_position(popup_row, popup_col);
  print_color_char(SINGLE_LINE_TOP_LEFT_CORNER, color, 1);
  set_cursor_position(popup_row, popup_col + 1);
  print_color_char(SINGLE_LINE_HORIZONTAL, color, title_left_padding - 2);
  set_cursor_position(popup_row, popup_col + title_left_padding - 1);
  print_color_char(' ', color, 1);
  print_color_string(title, color, popup_row, popup_col + title_left_padding);
  set_cursor_position(popup_row, popup_col + title_left_padding + title_length);
  print_color_char(' ', color, 1);
  set_cursor_position(popup_row, popup_col + title_left_padding + title_length + 1);
  print_color_char(SINGLE_LINE_HORIZONTAL, color, title_right_padding - 2);
  set_cursor_position(popup_row, popup_col + popup_width - 1);
  print_color_char(SINGLE_LINE_TOP_RIGHT_CORNER, color, 1);

  for (i = 0; i < 3; i++) {
    set_cursor_position(popup_row + 1 + i, popup_col);
    print_color_char(SINGLE_LINE_VERTICAL, color, 1);
    set_cursor_position(popup_row + 1 + i, popup_col + 1);
    print_color_char(' ', color, popup_width - 2);
    set_cursor_position(popup_row + 1 + i, popup_col + popup_width - 1);
    print_color_char(SINGLE_LINE_VERTICAL, color, 1);
    set_cursor_position(popup_row + 1 + i, popup_col + popup_width);
    print_color_char(BLOCK_FULL, COLOR(BLACK, BLACK), 2);
  }

  set_cursor_position(popup_row + 4, popup_col);
  print_color_char(SINGLE_LINE_BOTTOM_LEFT_CORNER, color, 1);
  set_cursor_position(popup_row + 4, popup_col + 1);
  print_color_char(SINGLE_LINE_HORIZONTAL, color, popup_width - 2);
  set_cursor_position(popup_row + 4, popup_col + popup_width - 1);
  print_color_char(SINGLE_LINE_BOTTOM_RIGHT_CORNER, color, 1);
  set_cursor_position(popup_row + 4, popup_col + popup_width);
  print_color_char(BLOCK_FULL, COLOR(BLACK, BLACK), 2);

  set_cursor_position(popup_row + 5, popup_col + 2);
  print_color_char(BLOCK_FULL, COLOR(BLACK, BLACK), popup_width);

  u8 input_color = COLOR(PASSIVE_TEXT, ACTIVE_BACKGROUND);

  print_color_string(input, input_color, popup_row + 2, popup_col + 3);
  print_color_string(unit_title, color, popup_row + 2, popup_col + 3 + input_length + 1);
  set_cursor_position(popup_row + 2, popup_col + 3 + input_length - 1);
}

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
) {
  int i, j;
  for (i = 0; i < num_main_entries; i++) {
    u8 color = selected_main_entry == i ? selected_sub_entry != -1 ?
      COLOR(PASSIVE_TEXT, PASSIVE_BACKGROUND) :
      COLOR(PASSIVE_TEXT, ACTIVE_BACKGROUND) :
      COLOR(ACTIVE_TEXT, BACKGROUND);
    print_color_string(
      get_string(main_menu_entries[i].name_id),
      color,
      6 + i,
      4
    );

    if (selected_main_entry == i) {
      for (j = 0; j < main_menu_entries[i].num_submenus; j++) {
        main_menu_entry main_menu_entry = main_menu_entries[i];
        color = COLOR(ACTIVE_TEXT, BACKGROUND);
        if (selected_sub_entry == j) {
          if (selected_change_entry != -1 || input != NULL || selected_quit_entry != -1) {
            color = COLOR(PASSIVE_TEXT, PASSIVE_BACKGROUND);
          } else {
            color = COLOR(PASSIVE_TEXT, ACTIVE_BACKGROUND);
          }
        }
        submenu_entry submenu_entry = main_menu_entry.submenus[j];
        if (submenu_entry.get_enabled != NULL && submenu_entry.get_enabled(lbs) == 0) {
          color = COLOR(PASSIVE_TEXT, PASSIVE_BACKGROUND);
        }
        if (submenu_entry.get_input_value != NULL) {
          u16 num_value = submenu_entry.get_input_value(lbs);
          char string_value[10];
          snprintf(string_value, 10, "  %4d", num_value);
          print_name_and_value(
            get_string(submenu_entry.name_id),
            string_value,
            color,
            6 + j,
            43
          );
        } else {
          u16 value = submenu_entry.get_selection_value(lbs);
          change_entry change_entry = get_change_entry(
            submenu_entry.change_entries,
            submenu_entry.num_change_entries,
            value
          );
          print_name_and_value(
            get_string(submenu_entry.name_id),
            change_entry.name,
            color,
            6 + j,
            43
          );
        }
      }
    }

    if (selected_sub_entry != -1) {
      draw_bottom_text(main_menu_entries[selected_main_entry].submenus[selected_sub_entry]);
    }
  }
  if (selected_main_entry != -1 && selected_sub_entry != -1 &&
      (selected_change_entry != -1 || input != NULL)) {

    main_menu_entry main_menu_entry = main_menu_entries[selected_main_entry];
    submenu_entry submenu_entry = main_menu_entry.submenus[selected_sub_entry];
    if (submenu_entry.get_input_value != NULL) {
      draw_input_popup(
        get_string(submenu_entry.name_id),
        submenu_entry.input_unit,
        input
      );
    } else {
      draw_selection_popup(
        main_menu_entries[selected_main_entry].submenus[selected_sub_entry],
        selected_change_entry
      );
    }
  }
  if (selected_quit_entry != -1) {
    draw_selection_popup(
      quit_menu_entry,
      selected_quit_entry
    );
  }
  set_cursor_position(25, 0);  // move the cursor below the last line
}

u32 last_rpm_draw = 0;
void draw_fan_rpms(u8 reset)
{
  if (reset) {
    last_rpm_draw = 0;
    return;
  }
  if (!timer_check(last_rpm_draw)) return;
  last_rpm_draw = timer_calc(500);

  u8 color = COLOR(PASSIVE_TEXT, BACKGROUND);
  print_color_string("Fan 1 Measured RPM: ", color, 10, 43);
  print_color_string("Fan 2 Measured RPM: ", color, 11, 43);
  print_color_string("Fan 3 Measured RPM: ", color, 12, 43);

  char rpm[5];
  char off[5] = " OFF";

  u8 fan1_status;
  u8 fan2_status;
  u8 fan3_status;

  llama_get_fan_statuses(&fan1_status, &fan2_status, &fan3_status);

  if (fan1_status & 0x07) {
    print_color_string(off, color, 10, 63);
  } else {
    u16_to_string(llama_get_fan1_tach_rpm(), rpm, 4);
    print_color_string(rpm, color, 10, 63);
  }
  if (fan2_status & 0x07) {
    print_color_string(off, color, 11, 63);
  } else {
    u16_to_string(llama_get_fan2_tach_rpm(), rpm, 4);
    print_color_string(rpm, color, 11, 63);
  }
  if (fan3_status & 0x07) {
    print_color_string(off, color, 12, 63);
  } else {
    u16_to_string(llama_get_fan3_tach_rpm(), rpm, 4);
    print_color_string(rpm, color, 12, 63);
  }
}