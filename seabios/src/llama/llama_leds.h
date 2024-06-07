#ifndef __LLAMA_LEDS_H
#define __LLAMA_LEDS_H

#include "types.h"

void llama_set_leds_enabled(u8 leds_enabled);
u8 llama_get_leds_enabled(void);
void llama_set_leds_intensity(u8 leds_intensity);
u8 llama_get_leds_intensity(void);
void llama_set_click_enabled(u8 click_enabled);
u8 llama_get_click_enabled(void);

#endif // __LLAMA_LEDS_H