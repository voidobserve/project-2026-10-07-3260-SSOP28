#include "io_key.h"

#include "user_config.h"

#if IO_KEY_ENABLE

#define IO_KEY_NO_KEY (255)

extern u8 io_key_get_key_id(void);
volatile io_key_para_t io_key_para = {
    0, IO_KEY_NO_KEY, 0, IO_KEY_FILTER_TIMES, 0, 0,
};

void io_key_config(void)
{
    P0_MD0 &= ~(GPIO_P02_MODE_SEL(0x03)); // 输入模式
}

u8 io_key_get_key_id(void)
{
    if (IO_KEY_PIN == 0) {
// 低电平表示按键按下
#if USER_DEBUG_ENABLE
        // printf("key press\n");
#endif
        return IO_KEY_ID_VALID;
    }

    return IO_KEY_NO_KEY;
}

void io_key_handle(void)
{
    u8 io_key_event = IO_KEY_EVENT_NONE;
    u8 key_value = IO_KEY_NO_KEY;

    if (io_key_para.cur_scan_times < IO_KEY_SCAN_CIRCLE_TIMES) {
        return;
    }

    io_key_para.cur_scan_times = 0;

    key_value = io_key_get_key_id();
    // 滤波
    if (key_value != io_key_para.filter_value) {
        if (io_key_para.filter_cnt < io_key_para.filter_time) {
            io_key_para.filter_cnt++;
            return;
        }

        io_key_para.filter_value = key_value;
        io_key_para.filter_cnt = 0;
    } else {
        io_key_para.filter_cnt = 0;
    }

    if (io_key_para.filter_value == IO_KEY_NO_KEY) {
        if (io_key_para.press_state == 1) {
            io_key_event = IO_KEY_EVENT_CLICK;
        }

        io_key_para.press_state = 0;
        io_key_para.press_time_ms = 0;
    } else if (io_key_para.filter_value == IO_KEY_ID_VALID) {
        if (io_key_para.press_state == 0) {
            io_key_para.press_state = 1;
            io_key_para.press_time_ms = 0;
            return;
        }

        if (io_key_para.press_state == 1) {
            if (io_key_para.press_time_ms <
                (((u16)-1) - IO_KEY_SCAN_CIRCLE_TIMES)) {
                io_key_para.press_time_ms += IO_KEY_SCAN_CIRCLE_TIMES;
            }

            if (io_key_para.press_time_ms >=
                IO_KEY_LONG_PRESS_TIME_THRESHOLD_MS) {
                io_key_para.press_state = 2;
                io_key_event = IO_KEY_EVENT_LONG;
            }
        }
    }

    if (io_key_event == IO_KEY_EVENT_NONE) {
        return;
    }

    switch (io_key_event) {
    case IO_KEY_EVENT_CLICK:
#if USER_DEBUG_ENABLE
        printf("click\n");
#endif
        break;
    case IO_KEY_EVENT_LONG:
#if USER_DEBUG_ENABLE
        printf("long\n");
#endif
        break;

    default:
        break;
    }
}

#endif
