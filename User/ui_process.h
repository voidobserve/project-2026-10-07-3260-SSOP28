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
#define UI_RPOCESS_FUEL_LEVEL_WHEN_CLOSE_PERIOD ((u16)5000)

// 油量挡位(包括低油量提示的挡位)
#define FUEL_LEVEL_MAX (6)

/*
	油量百分比均分为 6 档，对应显示：

	0%  ~ 16% ：低油量提示（油桶常亮 + 第 0 格红灯闪烁）
	17% ~ 33% ：两格指示灯
	34% ~ 50% ：三格指示灯
	51% ~ 66% ：四格指示灯
	67% ~ 83% ：五格指示灯
	84% ~ 100%：六格指示灯

	其中 5%(含)以下 或者 没有接油量(0xFF)：油桶与红灯一起闪烁（几乎没有油量）
*/ 
enum
{
    FUEL_LEV_0 = 100 / FUEL_LEVEL_MAX,     // 低油量档的上界
    FUEL_LEV_1 = 100 * 2 / FUEL_LEVEL_MAX, // 两格指示灯档的上界
    FUEL_LEV_2 = 100 * 3 / FUEL_LEVEL_MAX,
    FUEL_LEV_3 = 100 * 4 / FUEL_LEVEL_MAX,
    FUEL_LEV_4 = 100 * 5 / FUEL_LEVEL_MAX,
    FUEL_LEV_5 = 100 * 6 / FUEL_LEVEL_MAX,
};

// 几乎没有油量的阈值：百分比小于等于该值（或没有接油量 0xFF）时，油桶与红灯一起闪烁
#define FUEL_LEV_EMPTY_TH ((u8)5)

// 油量挡位边界的回差，单位：百分比
// 升档需超过 (边界 + 回差)，降档需落回 (边界 - 回差) 以下，避免读数压在边界上时格数来回跳
#define FUEL_LEV_HYST ((u8)2)

// 没有历史挡位值（上电首次检测）：不做回差，直接按百分比取档
#define FUEL_LEV_UNKNOWN ((u8)0xFF)

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

