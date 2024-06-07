#ifndef __LLAMA_FONTS_H
#define __LLAMA_FONTS_H

#include "types.h"

#define DOUBLE_LINES_TOP_LEFT_CORNER        0xC9
#define DOUBLE_LINES_TOP_RIGHT_CORNER       0xBB
#define DOUBLE_LINES_BOTTOM_LEFT_CORNER     0xC8
#define DOUBLE_LINES_BOTTOM_RIGHT_CORNER    0xBC
#define DOUBLE_LINES_HORIZONTAL             0xCD
#define DOUBLE_LINES_VERTICAL               0xBA
#define DOUBLE_LINES_VERTICAL_DL_RIGHT      0xCC
#define DOUBLE_LINES_VERTICAL_DL_LEFT       0xB9

#define DOUBLE_LINES_HORIZONTAL_SL_DOWN     0xD1
#define DOUBLE_LINES_VERTICAL_SL_RIGHT      0xC7
#define DOUBLE_LINES_VERTICAL_SL_LEFT       0xB6

#define SINGLE_LINE_TOP_LEFT_CORNER         0xDA
#define SINGLE_LINE_TOP_RIGHT_CORNER        0xBF
#define SINGLE_LINE_BOTTOM_LEFT_CORNER      0xC0
#define SINGLE_LINE_BOTTOM_RIGHT_CORNER     0xD9
#define SINGLE_LINE_HORIZONTAL              0xC4
#define SINGLE_LINE_HORIZONTAL_SL_UP        0xC1
#define SINGLE_LINE_VERTICAL                0xB3

#define SINGLE_LINE_CROSS                   0xC5

#define BLOCK_FULL                          0xDB

#define LLAMA_FONT_0                        0xD0
#define LLAMA_FONT_1                        0xD2
#define LLAMA_FONT_UP_ARROW                 0xE0
#define LLAMA_FONT_DOWN_ARROW               0xE1
#define LLAMA_FONT_LEFT_ARROW               0xE2
#define LLAMA_FONT_RIGHT_ARROW              0xE3

#define LLAMA_FONT_0_ARRAY_POS              (16 * 0)
#define LLAMA_FONT_1_ARRAY_POS              (16 * 1)
#define LLAMA_FONT_UP_ARROW_ARRAY_POS       (16 * 8)

extern u8 llama_fonts[192] VARLOW;
void load_custom_fonts(u8 *font_ptr, u16 ascii_position, u16 count);

#endif // __LLAMA_FONTS_H