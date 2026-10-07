#include "io_key.h"

#include "user_config.h"

#include "key_driver.h"

#include "instrument.h"

#if IO_KEY_ENABLE

extern u8 io_key_get_key_id(void);
// volatile key_driver_para_t ad_key_para = {
volatile struct key_driver_para io_key_para = {
    // 编译器不支持指定成员赋值的写法，会报错
    IO_KEY_SCAN_CIRCLE_TIMES,
    0,
    // NO_KEY,
    0,

    0,
    0,
    3,

    IO_KEY_LONG_PRESS_TIME_THRESHOLD_MS / IO_KEY_SCAN_CIRCLE_TIMES,
    (IO_KEY_LONG_PRESS_TIME_THRESHOLD_MS +
     IO_KEY_HOLD_PRESS_TIME_THRESHOLD_MS) /
        IO_KEY_SCAN_CIRCLE_TIMES,
    0,

    0,
    0,
    // 200 / IO_KEY_SCAN_CIRCLE_TIMES,
    0,
    // NO_KEY,
    0,
    KEY_TYPE_TOUCH, // 触摸按键，实际上是检测触摸ic传递过来的电平信号，可以当作io来检测
    io_key_get_key_id,

    IO_KEY_ID_NONE,
    KEY_EVENT_NONE,
};

#define IO_KEY_EFFECT_EVENT_NUMS 4
static const u8 io_key_event_table[][IO_KEY_EFFECT_EVENT_NUMS + 1] = {
    IO_KEY_ID_VALID,   IO_KEY_EVENT_CLICK, IO_KEY_EVENT_LONG,
    IO_KEY_EVENT_HOLD, IO_KEY_EVENT_LOOSE,
};

void io_key_config(void)
{
    P0_MD1 &= ~(GPIO_P05_MODE_SEL(0x03)); // 输入模式
}

u8 io_key_get_key_id(void)
{
    if (IO_KEY_PIN == 0) {
        // 引脚配置为输入上拉，低电平表示按键按下
        return IO_KEY_ID_VALID;
    }

    return NO_KEY;
}

u8 __io_key_get_event__(u8 key_val, u8 key_event)
{
    u8 ret_key_event = IO_KEY_EVENT_NONE;
    u8 i = 0;
    u8 index = 0;

    if (key_event == KEY_EVENT_CLICK) {
        index = 1;
    } else if (key_event == KEY_EVENT_LONG) {
        index = 2;
    } else if (key_event == KEY_EVENT_HOLD) {
        index = 3;
    } else if (key_event == KEY_EVENT_UP) {
        index = 4;
    }

    for (; i < ARRAY_SIZE(io_key_event_table); i++) {
        if (key_val == io_key_event_table[i][0]) {
            ret_key_event = io_key_event_table[i][index];
            break;
        }
    }

    return ret_key_event;
}

void io_key_handle(void)
{
    u8 io_key_event = IO_KEY_EVENT_NONE;

    if (io_key_para.latest_key_val == IO_KEY_ID_NONE) {
        return;
    }

    io_key_event = __io_key_get_event__(io_key_para.latest_key_val,
                                        io_key_para.latest_key_event);
    io_key_para.latest_key_val = IO_KEY_ID_NONE;
    io_key_para.latest_key_event = KEY_EVENT_NONE;

    switch (io_key_event) {
    case KEY_EVENT_CLICK:
#if USER_DEBUG_ENABLE
        // printf("click\n");
#endif
        instrument.key_event = 0x00; // 表示按键短按事件
        instrument_info_report_key_event_set();
        break;
    case IO_KEY_EVENT_LONG:
#if USER_DEBUG_ENABLE
        // printf("Long\n");
#endif
        instrument.key_event = 0x01; // 表示按键长按事件
        instrument_info_report_key_event_set();
        break;

    default:
        break;
    }
}

#endif
