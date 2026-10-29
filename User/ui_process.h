#ifndef __UI_PROCESS_H__
#define __UI_PROCESS_H__

#include "typedef.h"

// ui 刷新 时速 的周期，单位：ms
#define UI_RPOCESS_SPEED_PERIOD 75

// ui 刷新 发动机转速 的周期，单位：ms
#define UI_RPOCESS_ENGINE_SPEED_PERIOD 10

// ui 刷新 油量挡位 的周期，单位：ms
#define UI_RPOCESS_FUEL_LEVEL_PERIOD ((u16)5000)
// 如果油量接近，刷新 油量挡位 的周期，单位：ms
#define UI_RPOCESS_FUEL_LEVEL_WHEN_CLOSE_PERIOD ((u16)20000)

// 油量挡位(包括低油量提示的挡位)
#define FUEL_LEVEL_MAX (6)

/*
	将油量挡位与油量百分比建立映射关系

	0 ~ 16,包括 16 , 低油量提示
	17 ~ 33,包括 33 , 1 格油量
	34 ~ 50,包括 50 , 2 格油量
	51 ~ 66,包括 66 , 3 格油量
	67 ~ 83,包括 83 , 4 格油量
	84 ~ 100,包括 100 , 5 格油量
*/ 
enum
{
    FUEL_LEV_0 = 100 / FUEL_LEVEL_MAX,     // 低油量,或者是没有接油量
    FUEL_LEV_1 = 100 * 2 / FUEL_LEVEL_MAX, // 至少有 1 格油量
    FUEL_LEV_2 = 100 * 3 / FUEL_LEVEL_MAX,
    FUEL_LEV_3 = 100 * 4 / FUEL_LEVEL_MAX,
    FUEL_LEV_4 = 100 * 5 / FUEL_LEVEL_MAX,
    FUEL_LEV_5 = 100 * 6 / FUEL_LEVEL_MAX,
};

void ui_process_speed_period_add(void);
void ui_process_speed(void);

void ui_process_engine_speed_frame_wave_10ms_isr(void);
void ui_process_engine_speed_frame_wave(void);

void ui_process_engine_speed_period_add(void);
void ui_process_engine_speed(void);

void ui_process_fuel_lev_period_add(void);
void ui_process_fuel_lev(void);

void ui_process_mileage(void);


void ui_process_boot_anim_timer_isr(void);
void ui_process_boot_anim(void);

#endif

