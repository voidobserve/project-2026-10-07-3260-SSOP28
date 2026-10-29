#ifndef __UI_H__
#define __UI_H__

#include "typedef.h"

/*
    处于设置界面，等待超时的时间：(单位：ms)
    实际测试样机是闪29次指示灯后退出设置页面(参数填29)，
    样机闪烁跟错误提示使用同一个时基：
*/
#define UI_SETTING_AUTO_EXIT_TIME ((u16)29 * 1000)
// 处于设置界面，指示灯闪烁的时间间隔：(单位：ms)
#define UI_SETTING_BLINK_PERIOD ((u16)1000)
// 有异常、错误提示时，对应的指示灯的闪烁时间间隔，单位：ms
#define UI_ERR_BLINK_PERIOD ((u16)1000)

enum
{
    UI_STATE_NORMAL,
    UI_STATE_SETTING_TIME_HOUR,   // 设置 时间 小时
    UI_STATE_SETTING_TIME_MINUTE, // 设置 时间 分钟
};
typedef u8 ui_state_t;

typedef struct
{
    u16 setting_blink_cnt;          // 设置指示灯闪烁的计时器
    u16 err_blink_time_cnt;         // 错误闪烁的计时器
    u16 auto_exit_setting_time_cnt; // 自动退出设置界面的计时器

    u8 blink_dir;

    u8 flag_is_setting_blink; // 设置指示灯闪烁的时间到来
    u8 flag_is_err_blink;     // 错误闪烁的时间到来
    u8 flag_is_auto_exit;     // 自动退出设置界面的时间到来

    ui_state_t state;
} ui_manager_t;

extern volatile ui_manager_t ui_manager;

void ui_manager_init(void);
void ui_timer_handle_isr(void);

void ui_set_state(ui_state_t state);

void ui_display_handle(void);
void ui_display_err_handle(void);
void ui_display_refresh(void);

#endif
