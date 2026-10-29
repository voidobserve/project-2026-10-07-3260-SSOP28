// user_config.h -- 自定义的配置文件
#ifndef __USER_CONFIG_H__
#define __USER_CONFIG_H__

#include "typedef.h"

#define USER_DEBUG_ENABLE 0
#if USER_DEBUG_ENABLE
// #define DEBUG_PIN P31
#endif

#define TOUCH_KEY_ENABLE 0 // 是否使能触摸按键检测功能(官方SDK的触摸按键)

#define SPEED_SCAN_ENABLE         1 // 是否使能速度检测功能
#define ENGINE_SPEED_SCAN_ENABLE  1 // 是否使能发动机转速检测功能
#define FUEL_CAPACITY_SCAN_ENABLE 1 // 是否使能油量检测功能

#define PIN_LEVEL_SCAN_ENABLE 1 // 是否使能引脚电平检测功能

#define SYS_ENTER_CRITICAL_SECTION() sys_enter_critical_section()
#define SYS_EXIT_CRITICAL_SECTION()  sys_exit_critical_section()

extern void sys_enter_critical_section(void);
extern void sys_exit_critical_section(void);

#if USER_DEBUG_ENABLE
#include <stdio.h> //
#endif

#endif // end file
