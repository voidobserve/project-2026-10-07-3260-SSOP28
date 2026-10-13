// user_config.h -- 自定义的配置文件
#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#include "typedef.h"

#define USER_DEBUG_ENABLE 1
#if USER_DEBUG_ENABLE
// #define DEBUG_PIN P31
#endif

#define TOUCH_KEY_ENABLE 0 // 是否使能触摸按键检测功能

#define SPEED_SCAN_ENABLE         1 // 是否使能速度检测功能
#define ENGINE_SPEED_SCAN_ENABLE  1 // 是否使能发动机转速检测功能
#define FUEL_CAPACITY_SCAN_ENABLE 0 // 是否使能油量检测功能

#define PIN_LEVEL_SCAN_ENABLE 1 // 是否使能引脚电平检测功能

#define USE_INTERNAL_FLASH_SAVE_DATA 0

extern volatile u8 is_main_adc_val_updated;
extern volatile u16 main_adc_val; // TEST ONLY

extern volatile u8 irq_was_enabled; // 存放中断使能标志寄存器值的变量

#define ENTER_CRITICAL_SECTION()                                               \
    do {                                                                       \
        irq_was_enabled = IE_EA;                                               \
        IE_EA = 0;                                                             \
    } while (0)
#define EXIT_CRITICAL_SECTION()                                                \
    do {                                                                       \
        IE_EA = irq_was_enabled;                                               \
    } while (0)

#if USER_DEBUG_ENABLE
#include <stdio.h> //
#endif

#endif // end file
