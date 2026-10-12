#ifndef __IO_KEY_H__
#define __IO_KEY_H__

#include "include.h" // 使用芯片官方提供的头文件
#include "user_config.h"

#define IO_KEY_ENABLE 1

#if IO_KEY_ENABLE

#define IO_KEY_PIN P02

#define IO_KEY_SCAN_CIRCLE_TIMES            (10)
#define IO_KEY_FILTER_TIMES                 (3)    // 触摸按键消抖次数
#define IO_KEY_LONG_PRESS_TIME_THRESHOLD_MS (5000) // 长按时间阈值（单位：ms）
#define IO_KEY_HOLD_PRESS_TIME_THRESHOLD_MS                                    \
    (150) // 长按持续(不松手)的时间阈值(单位：ms)，每隔 xx 时间认为有一次长按持续事件

enum
{
    IO_KEY_ID_NONE = 0,
    IO_KEY_ID_VALID = 1,
};

enum
{
    IO_KEY_EVENT_NONE = 0,
    IO_KEY_EVENT_CLICK = 1,
    IO_KEY_EVENT_LONG = 2,
};

typedef struct
{
    volatile u8 cur_scan_times;
    volatile u8 filter_value;
    volatile u8 filter_cnt;
    volatile u8 filter_time;
    volatile u8 press_state;
    volatile u16 press_time_ms;
} io_key_para_t;

extern volatile io_key_para_t io_key_para;

void io_key_config(void);
void io_key_handle(void);

#endif

#endif