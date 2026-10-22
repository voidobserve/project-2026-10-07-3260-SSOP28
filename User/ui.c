#include "ui.h"
#include "include.h"
#include "aip3368_driver.h"
#include "aip3368h_display.h"
#include "aip1302.h"
#include "instrument.h"

#include "user_config.h"

// ui管理结构体
volatile ui_manager_t ui_manager = {0};

void ui_manager_init(void)
{
    ui_manager.state = UI_STATE_NORMAL;
}

/**
 * @brief ui定时器处理
 *
 * @attention 在定时器调用，负责所有ui相关的计数器处理
 *
 */
void ui_timer_handle_isr(void)
{
    aip3368h_refresh_time_add(); // 控制将显存数据刷新到屏幕驱动ic的周期

    if (ui_manager.err_blink_time_cnt < ((u16)-1)) {
        ui_manager.err_blink_time_cnt++;
        if (ui_manager.err_blink_time_cnt >= UI_ERR_BLINK_PERIOD) {
            ui_manager.err_blink_time_cnt = 0;
            ui_manager.flag_is_err_blink = 1;
        }
    }

    // 如果在设置界面
    if (ui_manager.state == UI_STATE_SETTING_TIME_MINUTE ||
        ui_manager.state == UI_STATE_SETTING_TIME_HOUR) {
        if (ui_manager.setting_blink_cnt < ((u16)-1)) {
            ui_manager.setting_blink_cnt++;
            if (ui_manager.setting_blink_cnt >= UI_SETTING_BLINK_PERIOD) {
                ui_manager.setting_blink_cnt = 0;
                ui_manager.flag_is_setting_blink = 1;
            }
        }

        if (ui_manager.auto_exit_setting_time_cnt < ((u16)-1)) {
            ui_manager.auto_exit_setting_time_cnt++;
            if (ui_manager.auto_exit_setting_time_cnt >=
                UI_SETTING_AUTO_EXIT_TIME) {
                ui_manager.auto_exit_setting_time_cnt = 0;
                ui_manager.flag_is_auto_exit = 1;
            }
        }
    } else {
        ui_manager.setting_blink_cnt = 0;
        ui_manager.auto_exit_setting_time_cnt = 0;

        ui_manager.flag_is_setting_blink = 0;
        ui_manager.flag_is_auto_exit = 0;
    }

#if 0
    // 递增 AIP3368H 显示 速度 刷新时间计数
    aip3368h_display_speed_refresh_time_add();
    // 递增 AIP3368H 显示 发动机转速 刷新时间计数
    aip3368h_display_engine_speed_refresh_time_add();
    ui_fuel_process_cnt_add();

#endif
}

// 设置ui状态（切换ui）
void ui_set_state(ui_state_t state)
{
    ui_manager.state = state;
    ui_manager.blink_dir = 0; // 初始化闪烁方向
}

/**
 * @brief 显示错误提示，由 ui_display_handle() 调用
 *
 */
void ui_display_err_handle(void)
{
    static volatile u8 display_err_dir = 0;
    u8 i;

    if (instrument.gear == GEAR_UNKNOWN) {
        if (0 == display_err_dir) {
            // 清除挡位显示，防止显示残留
            for (i = 0; i < 7; i++) {
                AIP3368H_DISPLAY_BUF_CLR(gear_map[i].buff_index,
                                         gear_map[i].bit_offset);
            }
        } else {
            AIP3368H_DISPLAY_BUF_SET(gear_map[SEG_IDX_G].buff_index,
                                     gear_map[SEG_IDX_G].bit_offset);
        }
    }

    // 时间冒号闪烁（样机的时间分隔符与错误提示共用一个时基）
    if (0 == display_err_dir) {
        aip3368h_display_time_colon_light(0);
    } else {
        aip3368h_display_time_colon_light(1);
    }

    if (0 == ui_manager.flag_is_err_blink) {
        return;
    }
    ui_manager.flag_is_err_blink = 0;

#if 0
// TODO 低油量时，要跟其他错误显示错开，时间相位刚好相反 
    // 低油量 提示
    if (instrument.flag_is_in_warning_of_low_fuel) {
        // 直接操作显存，判断指示灯是否点亮，进而让它闪烁
        // 让第 0 格油量的指示灯和油量图标指示灯一起闪烁

        if (0 == display_err_dir) {
            // 油量 图标
            aip3368h_display_fuel_icon_light(0);
            // 油量 第 0 格 指示灯
            __aip3368h_display_fuel_lev__(0, 0);
        } else {
            // 油量 图标
            aip3368h_display_fuel_icon_light(1);
            // 油量 第 0 格 指示灯
            __aip3368h_display_fuel_lev__(0, 1);
        }
    } else {
        // 油量 图标
        aip3368h_display_fuel_icon_light(1);
    }
#endif

    display_err_dir = !display_err_dir;
}

// 显示处理
void ui_display_handle(void)
{
    switch (ui_manager.state) {
    case UI_STATE_NORMAL:
        // 正常显示时

        // REVIEW 在测试时会屏蔽
        aip3368h_display_time_hour(1, aip1302_info.time_hour);
        aip3368h_display_time_minute(1, aip1302_info.time_min);

        break;

    case UI_STATE_SETTING_TIME_MINUTE:
        // 设置 时间 分钟

        // 设置分钟期间，不显示小时
        aip3368h_display_time_hour(0, 0);

        if (0 == ui_manager.flag_is_setting_blink) {
            // 设置期间，闪烁周期还没有到来，跳过闪烁
            break;
        }
        ui_manager.flag_is_setting_blink = 0;

        aip3368h_display_time_minute(ui_manager.blink_dir,
                                     aip1302_info.time_min);
        ui_manager.blink_dir = !ui_manager.blink_dir;
        break;

    case UI_STATE_SETTING_TIME_HOUR:

        // 设置小时期间，不显示分钟
        aip3368h_display_time_minute(0, 0);

        if (0 == ui_manager.flag_is_setting_blink) {
            // 设置期间，闪烁周期还没有到来，跳过闪烁
            break;
        }
        ui_manager.flag_is_setting_blink = 0;

        aip3368h_display_time_hour(ui_manager.blink_dir,
                                   aip1302_info.time_hour);
        ui_manager.blink_dir = !ui_manager.blink_dir;
        break;
    }

#if 0
   

    // 设置超时
    if (ui_manager.auto_exit_setting_time_cnt >= UI_SETTING_AUTO_EXIT_TIME) {
        ui_manager.auto_exit_setting_time_cnt = 0;

        if (UI_STATE_SETTING_TIME_MINUTE == ui_manager.state ||
            UI_STATE_SETTING_TIME_HOUR == ui_manager.state) {
            // 如果是设置时间，超时后，将时间写回
            // TODO
        }

        ui_set_state(UI_STATE_NORMAL);
        ui_display_refresh();

        // 自动退出设置界面后，保存相关数据
        instrument_info_save_enable();
    }

    aip3368h_display_gear(instrument.gear); // 显示档位
    ui_instruct_light_process();
    // TEST ONLY 测试时会屏蔽部分显示功能
    ui_speed_process();                     // 显示时速
    aip3368h_display_engine_speed_handle(); // 显示发动机转速

    aip3368h_display_mileage_refresh();
    ui_fuel_process();
    ui_bat_process();
#endif

    // 挡位有效时，显示挡位。否则进入显示错误提示处理函数
    if (instrument.gear != GEAR_UNKNOWN) {
        aip3368h_display_gear(instrument.gear);
    } else {
        aip3368h_display_gear_n_light(0);
    }

    ui_display_err_handle(); // 显示错误提示（例如低油量提示）
    aip3368h_module_display();
}

/**
 * @brief
 * 		清空所有计时
 * 		立即显示内容
 *
 * @note 只在切换了状态，需要立即刷新显示时使用，不能在每次显示时调用
 *
 */
void ui_display_refresh(void)
{
    ENTER_CRITICAL_SECTION();
    ui_manager.blink_dir = 0;
    ui_manager.setting_blink_cnt = 0;
    ui_manager.auto_exit_setting_time_cnt = 0;

    ui_manager.flag_is_setting_blink = 0;
    ui_manager.flag_is_err_blink = 0;
    ui_manager.flag_is_auto_exit = 0;
    EXIT_CRITICAL_SECTION();

#if 0
    switch (ui_manager.state) {
    case UI_STATE_NORMAL:
        // 正常显示时

        // 显示时速
        aip3368h_display_speed_by_unit_type(instrument.speed_to_display);

        break;
    }

    // 公制单位
    aip3368h_display_mph_light(0);
    aip3368h_display_kmh_light(1);
    // 立即更新里程显示：
    aip3368h_display_mileage_refresh();
    aip3368h_display_mileage_unit_lights(instrument.distance_unit_type);

#endif
    // 立即更新时间显示：
    aip3368h_display_time_hour(1, aip1302_info.time_hour);
    aip3368h_display_time_minute(1, aip1302_info.time_min);
}
