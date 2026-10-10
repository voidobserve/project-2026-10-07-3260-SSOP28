#ifndef __AIP3368H_DISPLAY_H__
#define __AIP3368H_DISPLAY_H__

#include "typedef.h"

#define AIP3368H_DISPLAY_TEST_ENABLE 1

void aip3368h_display_left_turn_light(u8 is_display);
void aip3368h_display_right_turn_light(u8 is_display);

#if AIP3368H_DISPLAY_TEST_ENABLE
void test_aip3368h_display(void);
#endif
#endif

