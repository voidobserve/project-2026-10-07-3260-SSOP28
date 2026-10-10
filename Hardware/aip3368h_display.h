#ifndef __AIP3368H_DISPLAY_H__
#define __AIP3368H_DISPLAY_H__

#include "typedef.h"

// TEST ONLY
#define AIP3368H_DISPLAY_TEST_ENABLE 1

#define AIP3368H_DISPLAY_BUF_SET(idx, bit_offset)                              \
    (aip3368h_display_buff[(idx)] |= (0x01 << (bit_offset)))

#define AIP3368H_DISPLAY_BUF_CLR(idx, bit_offset)                              \
    (aip3368h_display_buff[(idx)] &= ~(0x01 << (bit_offset)))

// 用于建立指示灯和显存的映射关系：
typedef struct
{
    // display_buff[] 中对应元素索引
    u8 buff_index;
    // display_buff[] 中对应元素中的第 x 位（按二进制数的排列方式，从右往左数）
    u8 bit_offset;
} aip3368h_display_mapping_t;

// 数码管 A ~ G 段索引值
enum
{
    SEG_IDX_A = 0x00,
    SEG_IDX_B,
    SEG_IDX_C,
    SEG_IDX_D,
    SEG_IDX_E,
    SEG_IDX_F,
    SEG_IDX_G,
};
typedef u8 seg_idx_t;

void aip3368h_display_left_turn_light(u8 is_display);
void aip3368h_display_right_turn_light(u8 is_display);
void aip3368h_display_high_beam_light(u8 is_display);
void aip3368h_display_low_beam_light(u8 is_display);

void aip3368h_display_clock_light(u8 is_display);
void aip3368h_display_time_colon_light(u8 is_display);
void aip3368h_display_time_hour(u8 is_display, u8 hour);
void aip3368h_display_time_minute(u8 is_display, u8 minute);
 
void aip3368h_display_mileage_point_light(u8 is_display);
void aip3368h_display_trip_light(u8 is_display);
void aip3368h_display_odo_light(u8 is_display);
void aip3368h_display_km_light(u8 is_display);
void aip3368h_display_mile_light(u8 is_display);
void aip3368h_display_mileage_bit_x(u8 is_display, u8 bit_x, u8 num);

void aip3368h_display_fuel_full_light(u8 is_display);
void aip3368h_display_fuel_empty_light(u8 is_display);
void aip3368h_display_fuel_icon_light(u8 is_display);
void aip3368h_display_fuel_lev_0_red_light(u8 is_display);
void aip3368h_display_fuel_lev(u8 is_display, u8 fuel_lev);

void aip3368h_display_engine_speed_frame_by_idx(u8 is_display, u8 idx);

#if AIP3368H_DISPLAY_TEST_ENABLE
void test_aip3368h_display_light_1ms_isr(void);
void test_aip3368h_display_engine_speed_lev_1ms_isr(void);
void test_aip3368h_display_engine_speed_frame_1ms_isr(void);
void test_aip3368h_display_engine_speed_frame_wave_1ms_isr(void);
void test_aip3368h_display_fuel_lev_1ms_isr(void);
void test_aip3368h_display_mileage_1ms_isr(void);
void test_aip3368h_display_time_1ms_isr(void);
void test_aip3368h_display(void);
#endif
#endif
