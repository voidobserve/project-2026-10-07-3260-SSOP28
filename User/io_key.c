#include "io_key.h"
#include "ui.h"
#include "instrument.h"
#include "aip1302.h"

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

        if (UI_STATE_NORMAL == ui_manager.state) {
            // 正常显示模式下，按键单击切换显示的里程类型
            instrument.is_display_total_mileage =
                !instrument.is_display_total_mileage;
            instrument_info_save_enable();
        } else if (UI_STATE_SETTING_TIME_HOUR == ui_manager.state) {
            // 在设置小时时，检测到短按
            aip1302_info.time_hour++;
            if (aip1302_info.time_hour > 23) {
                aip1302_info.time_hour = 0;
            }

#if USER_DEBUG_ENABLE
            // printf("hour == %u\n", (u16)aip1302_info.time_hour);
#endif
        } else if (UI_STATE_SETTING_TIME_MINUTE == ui_manager.state) {
            // 在设置分钟时，检测到短按
            aip1302_info.time_min++;
            if (aip1302_info.time_min > 59) {
                aip1302_info.time_min = 0;
            }
        }

        ui_display_refresh();

        break;
    case IO_KEY_EVENT_LONG:
#if USER_DEBUG_ENABLE
        printf("long\n");
#endif

        if (UI_STATE_NORMAL == ui_manager.state) {
            // 如果正在显示总里程
            if (1 == instrument.is_display_total_mileage) {
                // 如果显示的是 TOTAL 里程，切换到设置时间-小时
                ui_set_state(UI_STATE_SETTING_TIME_HOUR);
            } else {
                // 如果显示的是 TRIP 里程，清空它
                instrument.subtotal_mileage = 0;
                instrument_info_save_enable(); // 使能保存
            }
        } else if (UI_STATE_SETTING_TIME_HOUR == ui_manager.state) {
            // 当前在设置时钟-小时，切换到设置时钟-分钟
            ui_set_state(UI_STATE_SETTING_TIME_MINUTE);
            // 切换后，将时间数据写回时钟芯片
            aip1302_update_time(&aip1302_info);
        } else if (UI_STATE_SETTING_TIME_MINUTE == ui_manager.state) {
            // 当前在设置时钟-分钟，切换到正常显示
            ui_set_state(UI_STATE_NORMAL);
            // 切换后，将时间数据写回时钟芯片
            aip1302_update_time(&aip1302_info);
        }

        ui_display_refresh();
        break;

    default:
        break;
    }
}

#endif
