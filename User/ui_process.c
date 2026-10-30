#include "ui_process.h"
#include "typedef.h"
#include "tx8T326x.h"        // WDT_KEY
#include "tx8T326x_system.h" // WDT_KEY_VAL()
#include <string.h>          // memset

#include "aip3368_driver.h"
#include "aip3368h_display.h"
#include "instrument.h"

#include "user_utils.h"
#include "user_config.h"

// 时速显示对应的滑动平均滤波：
#define SPEED_FILTER_ARRAY_SIZE (40) // 滤波数组大小
// 不写显式初值：STARTUP.A51 启动时会把 XDATA 整片清零，
// 显式 = {0} 反而会在代码区生成一份初值镜像
static volatile u8 speed_filter_array[SPEED_FILTER_ARRAY_SIZE];
static volatile u8 speed_filter_index;

// ui处理时速显示的周期
static volatile u8 ui_process_speed_period;
// ui处理发动机转速边框动画的周期
static volatile u8 ui_process_engine_speed_frame_wave_period;
// ui处理发动机转速的周期计数器
static volatile u8 ui_process_engine_speed_period;
// ui处理 油量 显示的周期
static volatile u16 ui_process_fuel_lev_period;

static void speed_filter_init(u8 speed)
{
    u8 i;
    for (i = 0; i < SPEED_FILTER_ARRAY_SIZE; i++) {
        speed_filter_array[i] = speed;
    }

    speed_filter_index = 0;
}

static void speed_filter_add(u8 speed)
{
    speed_filter_array[speed_filter_index] = speed;
    speed_filter_index++;
    if (speed_filter_index >= SPEED_FILTER_ARRAY_SIZE) {
        speed_filter_index = 0;
    }
}

static u8 speed_filter_get(void)
{
    u8 i;
    u32 sum = 0;
    for (i = 0; i < SPEED_FILTER_ARRAY_SIZE; i++) {
        sum += speed_filter_array[i];
    }

    return (u8)(sum / SPEED_FILTER_ARRAY_SIZE);
}

void ui_process_speed_period_add(void)
{
    if (ui_process_speed_period < UINT8_T_MAX) {
        ui_process_speed_period++;
    }
}

void ui_process_speed(void)
{
    static volatile u8 is_initialized = 0; // 是否初始化
    volatile u8 cur_speed;                 //

    /*
        一阶低通滤波器配置，滤波系数 α ，范围：0 ~ 10，推荐值 1 ~ 3
        α越小越平滑但响应慢,α越大响应快但平滑差
    */
#define ALPHA 7 // 滤波系数 α
    static volatile u8 filtered_speed;
    u8 base_step;      // 显示时速期间，每次调节显示的步长
    u8 speed_abs_diff; // 存放当前显示的时速和当前时速的差值

    if (0 == is_initialized) {
        is_initialized = 1;

        // 初始化，直接获取当前最新的速度值
        instrument.speed_to_display = instrument.speed;
        filtered_speed = instrument.speed;
        aip3368h_display_speed(instrument.speed_to_display);
        speed_filter_init(instrument.speed);

        // 公制单位： km/h
        aip3368h_display_mph_light(0);
        aip3368h_display_kmh_light(1);
    }

    if (ui_process_speed_period >= UI_RPOCESS_SPEED_PERIOD) {
        ui_process_speed_period = 0;

        // 如果当前显示的速度值和计算出来的速度值相差太大，需要进行快速逼近：
        if (instrument.speed_to_display > instrument.speed) {
            speed_abs_diff = instrument.speed_to_display - instrument.speed;
            if (speed_abs_diff >= 10) {
                // 根据速度插值，调节步长
                if (speed_abs_diff >= 50) {
                    base_step = 20;
                } else if (speed_abs_diff >= 20) {
                    base_step = 10;
                } else {
                    base_step = 5;
                }

                instrument.speed_to_display -= base_step;
#if USER_DEBUG_ENABLE
                // printf("instrument.speed_to_display == %u\n", (u16)instrument.speed_to_display);
#endif

                speed_filter_init(instrument.speed);
                filtered_speed = instrument.speed;
                aip3368h_display_speed(instrument.speed_to_display);
                return;
            }
        } else if (instrument.speed_to_display < instrument.speed) {
            speed_abs_diff = instrument.speed - instrument.speed_to_display;
            if (speed_abs_diff >= 10) {
                // 根据速度插值，调节步长
                if (speed_abs_diff >= 50) {
                    base_step = 20;
                } else if (speed_abs_diff >= 20) {
                    base_step = 10;
                } else {
                    base_step = 5;
                }

                instrument.speed_to_display += base_step;
#if USER_DEBUG_ENABLE
                // printf("instrument.speed_to_display == %u\n", (u16)instrument.speed_to_display);
#endif

                speed_filter_init(instrument.speed);
                filtered_speed = instrument.speed;

                aip3368h_display_speed(instrument.speed_to_display);
                return;
            }
        }

        speed_filter_add(instrument.speed);
        cur_speed = speed_filter_get();
        /*
            如果当前速度值和过滤后的速度值都是10以下，需要注意
            不能再经过低通滤波器，会导致最后计算出的时速变为0
        */
        if (cur_speed < 10) {
            filtered_speed = cur_speed;
        } else {
            // 套用一阶低通滤波器计算公式：
            filtered_speed =
                ((u32)ALPHA * cur_speed + (10 - ALPHA) * (u32)filtered_speed) /
                10;
        }

        cur_speed = filtered_speed;

        if (instrument.speed_to_display > cur_speed) {
            instrument.speed_to_display--;
        } else if (instrument.speed_to_display < cur_speed) {
            instrument.speed_to_display++;
        }

#if USER_DEBUG_ENABLE
        // printf("speed_to_display == %u\n", (u16)speed_to_display);
#endif

        aip3368h_display_speed(instrument.speed_to_display);
    }
}

void ui_process_engine_speed_frame_wave_10ms_isr(void)
{
    if (ui_process_engine_speed_frame_wave_period < UINT8_T_MAX) {
        ui_process_engine_speed_frame_wave_period++;
    }
}

/**
 * @brief 点亮发动机转速边框中间的三个灯
 */
static void engine_speed_frame_wave_show_center(void)
{
    const u8 center_idx = 10; // 中间位置

    aip3368h_display_engine_speed_frame_by_idx(1, center_idx - 1);
    aip3368h_display_engine_speed_frame_by_idx(1, center_idx);
    aip3368h_display_engine_speed_frame_by_idx(1, center_idx + 1);
}

/**
 * @brief 点亮向两侧移动的四个灯
 *
 * @param spread 距离中间位置的扩散量
 */
static void engine_speed_frame_wave_show_spread(s8 spread)
{
    const u8 center_idx = 10; // 中间位置

    aip3368h_display_engine_speed_frame_by_idx(1, center_idx - 1 - spread);
    aip3368h_display_engine_speed_frame_by_idx(1, center_idx - 2 - spread);
    aip3368h_display_engine_speed_frame_by_idx(1, center_idx + 1 + spread);
    aip3368h_display_engine_speed_frame_by_idx(1, center_idx + 2 + spread);
}

/**
 * @brief 发动机转速边框动画：中间三盏灯向两侧扩散，再往中间回收，循环播放
 *
 *  动画过程：
 *  1. 从中间三个灯开始；
 *  2. 两个灯往左侧扩散，两个灯往右侧扩散；
 *  3. 到达边界后折返，向中间回收；
 *  4. 回到中间时，四个移动的灯会重合成中间三个灯，循环。
 */
void ui_process_engine_speed_frame_wave(void)
{
    static s8 dir = 0;       // 0：向两侧扩散，1：向中间回收
    static s8 spread = -1;   // -1：中间三个灯，0..8：向两侧扩散/回收
    const s8 max_spread = 8; // 扩散到边界位置
    u8 i;

    if (ui_process_engine_speed_frame_wave_period >= (500 / 10)) {
        ui_process_engine_speed_frame_wave_period = 0;
    } else {
        // 动画播放间隔未到，直接返回
        return;
    }

    // 清空当前所有发动机转速边框灯
    for (i = 0; i < AIP3368H_DISPLAY_ENGINE_SPEED_FRAME_NUMS; i++) {
        aip3368h_display_engine_speed_frame_by_idx(0, i);
    }

    if (spread < 0) {
        // 中间三个灯（扩散开始时、回收到中间时）
        engine_speed_frame_wave_show_center();
        dir = 0;
        spread = 0;
        return;
    }

    // 两个灯往左侧移动，两个灯往右侧移动
    engine_speed_frame_wave_show_spread(spread);

    if (0 == dir) {
        if (spread >= max_spread) {
            dir = 1;
            spread = max_spread - 1;
        } else {
            spread++;
        }
    } else {
        // 碰到边界后，直接开始向中间回收，不再多停一拍
        if (spread <= 0) {
            dir = 0;
            spread = -1;
        } else {
            spread--;
        }
    }
}

void ui_process_engine_speed_period_add(void)
{
    if (ui_process_engine_speed_period < UINT8_T_MAX) {
        ui_process_engine_speed_period++;
    }
}

/**
 * @brief 获取发动机转速挡位
 * 
 * @return u8 
 */
static u8 ui_process_engine_speed_get_lev(void)
{
    // 仪表上的一格对应500rpm，这里除以500，得到档位
    return (u8)(instrument.engine_speed / 500);
}

/**
 * @brief 显示发动机转速
 * 
 */
void ui_process_engine_speed(void)
{
    // 延迟显示的发动机转速
    static u8 engine_speed_level_of_lag = 0;
    // static u8 is_initialized = 0;
    u8 cur_engine_speed_level = 0;
    u8 level_diff;             // 显示的和实际计算得到的挡位插值
    u8 refresh_time_threshold; // 刷新间隔阈值

    // if (is_initialized == 0) {
    //     is_initialized = 1;
    //     engine_speed_level_of_lag = ui_process_engine_speed_get_lev();
    // }

    // 如果当前发动机转速与显示的发动机转速很接近，延长刷新时间（样机大约是2s）
    cur_engine_speed_level = ui_process_engine_speed_get_lev();
    if (cur_engine_speed_level == 0) {
        // 转速为0时，快速更新
        refresh_time_threshold = UI_RPOCESS_ENGINE_SPEED_PERIOD;
    } else {
        level_diff = (cur_engine_speed_level > engine_speed_level_of_lag)
                         ? (cur_engine_speed_level - engine_speed_level_of_lag)
                         : (engine_speed_level_of_lag - cur_engine_speed_level);

        // 转速挡位差值较大，快速更新；转速挡位差值较小，延长刷新时间
        refresh_time_threshold = (level_diff <= 1)
                                     ? (UI_RPOCESS_ENGINE_SPEED_PERIOD * 2)
                                     : UI_RPOCESS_ENGINE_SPEED_PERIOD;
    }

    if (ui_process_engine_speed_period >= refresh_time_threshold) {
        ui_process_engine_speed_period = 0;
        if (engine_speed_level_of_lag < cur_engine_speed_level) {
            engine_speed_level_of_lag++;
        } else if (engine_speed_level_of_lag > cur_engine_speed_level) {
            if (engine_speed_level_of_lag > 0) {
                engine_speed_level_of_lag--;
            }
        }

        aip3368h_display_engine_speed_lev(engine_speed_level_of_lag);
    }
}

void ui_process_fuel_lev_period_add(void)
{
    if (ui_process_fuel_lev_period < UINT16_T_MAX) {
        ui_process_fuel_lev_period++;
    }
}

static u8 fuel_percent_to_lev(u8 fuel_percent)
{
    u8 ret = 0;
    if (fuel_percent <= FUEL_LEV_0 || (fuel_percent == 0xFF)) {
        ret = 0;
    } else if (fuel_percent <= FUEL_LEV_1) {
        ret = 1;
    } else if (fuel_percent <= FUEL_LEV_2) {
        ret = 2;
    } else if (fuel_percent <= FUEL_LEV_3) {
        ret = 3;
    } else if (fuel_percent <= FUEL_LEV_4) {
        ret = 4;
    } else {
        ret = 5;
    }

    return ret;
}

/**
 * @brief 按油量挡位刷新格子显示
 * @param fuel_lev 油量挡位（0 ~ 5）：0 清空格子；1 ~ 5 显示 2 ~ 6 格
 * @note 没有只亮第 0 格蓝灯的显示：挡位 1 起显示 2 格（第 0 格 + 第 1 格）
 */
static void fuel_lev_show(u8 fuel_lev)
{
    if (fuel_lev == 0) {
        aip3368h_display_fuel_lev(0); // 低油量/几乎没有油量：格子全部熄灭
    } else {
        aip3368h_display_fuel_lev((u8)(fuel_lev + 1)); // 挡位 N 显示 N+1 格
    }
}

/**
 * @brief 根据显示的油量挡位与当前油量值，刷新油量状态
 * @param fuel_lev 当前显示的油量挡位（0 ~ 5）
 */
static void fuel_state_update(u8 fuel_lev)
{
    if (fuel_lev != 0) {
        instrument.fuel_state = FUEL_STATE_NORMAL;
        return;
    }
    // 挡位为 0：几乎没有油量(<=5%)、没有接油量(0xFF) 或 低油量
    if (instrument.fuel_percent == FUEL_PERCENT_NO_SENSOR ||
        instrument.fuel_percent <= FUEL_LEV_EMPTY_TH) {
        instrument.fuel_state = FUEL_STATE_EMPTY;
    } else {
        instrument.fuel_state = FUEL_STATE_LOW;
    }
}

void ui_process_fuel_lev(void)
{
    static u8 is_initialized = 0;               // 是否已按首次检测值刷新过
    static volatile u8 fuel_lev_to_display = 0; // 最终要显示的油量等级
    volatile u8 cur_fuel_lev = 0;               // 当前油量等级
    volatile u8 fuel_lev_diff = 0; // 当前的油量等级与显示的油量等级之间的差值
    volatile u16 process_period;

    SYS_ENTER_CRITICAL_SECTION();
    process_period = ui_process_fuel_lev_period;
    SYS_EXIT_CRITICAL_SECTION();

    if (0 == is_initialized) {
        // 上电动画之后，根据第一次检测到的油量值立即刷新显示
        if (0 == instrument.fuel_percent_valid) {
            return; // 还没有第一次检测值，继续等待
        }
        is_initialized = 1;
        cur_fuel_lev = fuel_percent_to_lev(instrument.fuel_percent);
        fuel_lev_to_display = cur_fuel_lev;
        fuel_lev_show(fuel_lev_to_display);
        fuel_state_update(fuel_lev_to_display);
        return;
    }

    cur_fuel_lev = fuel_percent_to_lev(instrument.fuel_percent);
    if (cur_fuel_lev != fuel_lev_to_display) {
        if (cur_fuel_lev > fuel_lev_to_display) {
            fuel_lev_diff = cur_fuel_lev - fuel_lev_to_display;
        } else {
            fuel_lev_diff = fuel_lev_to_display - cur_fuel_lev;
        }
    }

    if (fuel_lev_diff == 0) {
        // 油量等级没有变化，清空计数值，再返回
        SYS_ENTER_CRITICAL_SECTION();
        ui_process_fuel_lev_period = 0;
        SYS_EXIT_CRITICAL_SECTION();
        fuel_state_update(fuel_lev_to_display); // 等级不变时也刷新低油量/没油状态
        return;
    } else if ((fuel_lev_diff >= 2 &&
                process_period < UI_RPOCESS_FUEL_LEVEL_PERIOD) ||
               (fuel_lev_diff == 1 &&
                process_period < UI_RPOCESS_FUEL_LEVEL_WHEN_CLOSE_PERIOD)) {
        // 没有到油量挡位更新时间，直接返回（油量快速变化时每次也只变 1 格）
        fuel_state_update(fuel_lev_to_display);
        return;
    }

    // 到点：只走 1 格，重新计时
    SYS_ENTER_CRITICAL_SECTION();
    ui_process_fuel_lev_period = 0;
    SYS_EXIT_CRITICAL_SECTION();

    if (fuel_lev_to_display < cur_fuel_lev) {
        fuel_lev_to_display++;
    } else if (fuel_lev_to_display > cur_fuel_lev) {
        fuel_lev_to_display--;
    }

    fuel_lev_show(fuel_lev_to_display); // 挡位 N 显示 N+1 格，不会出现只亮第 0 格蓝灯
    // 油量状态（油桶图标、红灯）由 ui_display_err_handle() 统一显示
    fuel_state_update(fuel_lev_to_display);
}

/**
 * @brief 显示里程，包括里程的单位
 * 
 * @note 小计里程始终以0.1km显示；总里程超过9999.9km后改为整数km显示
 *      样机默认使用公制单位，没有切换英制单位的功能
 * 
 */
void ui_process_mileage(void)
{
    u32 display_mileage;
    u32 total_mileage;
    u8 display_total_mileage; // 是否显示总里程(ODO)
    u8 display_decimal;       // 是否显示小数

    display_total_mileage = instrument.is_display_total_mileage;
    if (display_total_mileage) {
        total_mileage = instrument.total_mileage;
        if (total_mileage > ((u32)99999 * 100)) {
            display_mileage = total_mileage / 1000;
            display_decimal = 0;
        } else {
            display_mileage = total_mileage / 100;
            display_decimal = 1;
        }
    } else {
        display_mileage = instrument.subtotal_mileage / 100;
        display_decimal = 1;
    }

    aip3368h_display_mileage(display_mileage);
    aip3368h_display_mileage_point_light(display_decimal); // 里程的小数点
    aip3368h_display_trip_light(!display_total_mileage);
    aip3368h_display_odo_light(display_total_mileage);

    aip3368h_display_mile_light(0);
    aip3368h_display_km_light(1);
}

static u8 is_boot_anim_finished = 0; // 是否已经完成上电动画
static u16 ui_process_boot_anim_cnt; // 直接用定时器1调用

// 上电动画使用到的定时器中断
void ui_process_boot_anim_timer_isr(void)
{
    if (is_boot_anim_finished) {
        return;
    }

    if (ui_process_boot_anim_cnt < UINT16_T_MAX) {
        ui_process_boot_anim_cnt++;
    }
}

/**
 * @brief 上电点亮所有指示灯
 * 
 * @return * void 
 */
void ui_process_boot_anim(void)
{
    // 点亮所有指示灯
    memset(aip3368h_display_buff, 0xFF, sizeof(aip3368h_display_buff));

    while (1) {
        WDT_KEY = WDT_KEY_VAL(0xAA); // 喂狗并清除 wdt_pending

        if (ui_process_boot_anim_cnt >= 2000) {
            is_boot_anim_finished = 1;
            // ui_process_boot_anim_cnt = 0; // 可以不用清零，后续不再使用

            // 有些使用不到的指示灯，需要熄灭
            aip3368h_display_abs_light(0);
            break;
        }

        aip3368h_module_display();
    }
}
