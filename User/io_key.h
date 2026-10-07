#ifndef __IO_KEY_H__
#define __IO_KEY_H__

#include "include.h" // 使用芯片官方提供的头文件
#include "user_config.h"

#define IO_KEY_ENABLE 0

#if IO_KEY_ENABLE

#define IO_KEY_PIN P05

#define IO_KEY_SCAN_CIRCLE_TIMES            (10)
#define IO_KEY_FILTER_TIMES                 (3)   // 触摸按键消抖次数
#define IO_KEY_LONG_PRESS_TIME_THRESHOLD_MS (500) // 长按时间阈值（单位：ms）
#define IO_KEY_HOLD_PRESS_TIME_THRESHOLD_MS                                    \
    (150) // 长按持续(不松手)的时间阈值(单位：ms)，每隔 xx 时间认为有一次长按持续事件

enum
{
    IO_KEY_ID_NONE = 0, // 无按键按下
    IO_KEY_ID_VALID,    // 有按键按下
};

enum
{
    IO_KEY_EVENT_NONE = 0,

    IO_KEY_EVENT_CLICK,
    IO_KEY_EVENT_LONG,
    IO_KEY_EVENT_HOLD,
    IO_KEY_EVENT_LOOSE,
};

extern volatile struct key_driver_para io_key_para;

void io_key_config(void);
void io_key_handle(void);

#endif

#endif