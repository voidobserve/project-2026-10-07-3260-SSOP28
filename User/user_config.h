// user_config.h -- 自定义的配置文件
#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

// #include "include.h" // 使用芯片官方提供的头文件
// #include "typedef_struct.h" // 包含结构体类型和别名的定义

#define ARRAY_SIZE(arry) (sizeof(arry) / sizeof(arry[0]))

#define USER_DEBUG_ENABLE 1
#if USER_DEBUG_ENABLE
// #define DEBUG_PIN P31
#endif

#define TOUCH_KEY_ENABLE 0 // 是否使能触摸按键检测功能

#define SPEED_SCAN_ENABLE         0 // 是否使能速度检测功能
#define ENGINE_SPEED_SCAN_ENABLE  0 // 是否使能发动机转速检测功能
#define BATTERY_SCAN_ENABLE       0 // 是否使能电池检测功能
#define FUEL_CAPACITY_SCAN_ENABLE 0 // 是否使能油量检测功能

#define PIN_LEVEL_SCAN_ENABLE 0 // 是否使能引脚电平检测功能

#define USE_INTERNAL_FLASH_SAVE_DATA 0

#define ENTER_CRITICAL_SECTION()                                               \
    do {                                                                       \
        IE_EA = 0;                                                             \
    } while (0)
#define EXIT_CRITICAL_SECTION()                                                \
    do {                                                                       \
        IE_EA = 1;                                                             \
    } while (0)

#if USER_DEBUG_ENABLE
#include <stdio.h> //
#endif

#endif // end file
