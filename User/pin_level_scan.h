#ifndef __PIN_LEVEL_SCAN_H__
#define __PIN_LEVEL_SCAN_H__

#include "include.h" // 使用芯片官方提供的头文件
#include "user_config.h"

#if PIN_LEVEL_SCAN_ENABLE

#define PIN_DETECT_GEAR_N (P06) // N 档 检测脚
#define PIN_DETECT_GEAR_1 (P07) // 1 档 检测脚
#define PIN_DETECT_GEAR_2 (P10) // 2 档 检测脚
#define PIN_DETECT_GEAR_3 (P13) // 3 档 检测脚
#define PIN_DETECT_GEAR_4 (P14) // 4 档 检测脚
#define PIN_DETECT_GEAR_5 (P30) // 5 档 检测脚
#define PIN_DETECT_GEAR_6 (P27) // 6 档 检测脚

#define PIN_DETECT_LEFT_TURN  (P25) // 左转向 检测脚
#define PIN_DETECT_RIGHT_TURN (P22) // 右转向 检测脚
#define PIN_DETECT_HIGH_BEAM  (P21) // 大灯（远光灯）检测脚

#define PIN_DETECT_TEMP_OF_WATER_ERR (P20) // 水温过热检测脚

// 定义信号有效时，检测脚对应的电平
#define SIGNAL_VALID_LEV_OF_GEAR_N (0)
#define SIGNAL_VALID_LEV_OF_GEAR_1 (0)
#define SIGNAL_VALID_LEV_OF_GEAR_2 (0)
#define SIGNAL_VALID_LEV_OF_GEAR_3 (0)
#define SIGNAL_VALID_LEV_OF_GEAR_4 (0)
#define SIGNAL_VALID_LEV_OF_GEAR_5 (0)
#define SIGNAL_VALID_LEV_OF_GEAR_6 (0)

#define SIGNAL_VALID_LEV_OF_LEFT_TURN  (0)
#define SIGNAL_VALID_LEV_OF_RIGHT_TURN (0)
#define SIGNAL_VALID_LEV_OF_HIGH_BEAM  (0)

#define SIGNAL_VALID_LEV_OF_TEMP_OF_WATER_ERR (0)

void pin_level_scan_config(void); // 扫描引脚的配置（初始化）
void pin_level_scan(void);        // 扫描引脚的电平
#endif

#endif
