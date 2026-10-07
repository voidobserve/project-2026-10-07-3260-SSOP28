#ifndef __KEY_DRIVER_H__
#define __KEY_DRIVER_H__

#include "user_config.h"

#include "typedef.h"

#define NO_KEY (255) // 无效按键的键值

enum KEY_EVENT
{
    KEY_EVENT_NONE = 0,
    KEY_EVENT_CLICK = 1,    // 单击
    KEY_EVENT_DOUBLE_CLICK, // 双击
    // KEY_EVENT_TRIPLE_CLICK, // 三击
    // KEY_EVENT_FOURTH_CLICK, // 四击
    // KEY_EVENT_FIRTH_CLICK,  // 五击
    KEY_EVENT_LONG, // 长按
    KEY_EVENT_HOLD, // 持续按下
    KEY_EVENT_UP,   // 长按/持续按下后，松开了按键
};

enum KEY_TYPE
{
    KEY_TYPE_NONE = 0,
    KEY_TYPE_AD,    // AD按键
    KEY_TYPE_TOUCH, // 触摸按键
};

typedef struct key_driver_para
{
    const u8 scan_times; // 按键扫描频率, 单位ms
    volatile u8
        cur_scan_times; // 按键扫描频率, 单位ms，由1ms的定时器中断内累加，在key_driver_scan()中清零
    volatile u8 last_key; // 存放上一次调用get_value()之后得到的按键值

    //== 用于消抖类参数 ==//
    volatile u8 filter_value; // 用于按键消抖，存放消抖期间得到的键值
    volatile u8 filter_cnt;   // 用于按键消抖时的累加值
    const u8 filter_time;     // 当filter_cnt累加到base_cnt值时, 消抖有效

    //== 用于判定长按和HOLD事件参数
    const u8 long_time; // 按键判定长按数量
    const u8 hold_time; // 按键判定HOLD数量
    volatile u8
        press_cnt; // 与long_time和hold_time对比, 判断long_event和hold_event

    //== 用于判定连击事件参数
    volatile u8 click_cnt;       // 按键按下次数
    volatile u8 click_delay_cnt; // 按键被抬起后等待连击事件延时计数
    const u8 click_delay_time;   // 按键被抬起后等待连击事件延时数量
    volatile u8 notify_value;    // 在延时的待发送按键值
    const u8 key_type;
    u8 (*get_value)(void); // 用户自定义的获取键值的函数

    // 存放得到的按键键值和按键事件
    volatile u8 latest_key_val;
    volatile u8 latest_key_event;
} key_driver_para_t;

extern void key_driver_scan(void *_scan_para);
// extern struct key_driver_para key_driver_scan(struct key_driver_para scan_para);

#endif
