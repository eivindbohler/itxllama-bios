#ifndef __LLAMA_CS4237_H
#define __LLAMA_CS4237_H

#include "llama/llama_bios_settings.h"

void cs4237_hostload(llama_bios_settings *lbs, u8 force_ext_fm);
void cs4237_slam_configuration(llama_bios_settings *lbs);
void cs4237_setup_mixer(llama_bios_settings *lbs);
void cs4237_enable_digital_output(llama_bios_settings *lbs);
u8 cs4237_test_opl3(llama_bios_settings *lbs);

#endif // __LLAMA_CS4237_H