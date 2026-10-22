#include "ui_process.h"
#include "typedef.h"

#include "aip3368h_display.h"
#include "instrument.h"

#include "user_utils.h"
#include "user_config.h"

// 时速显示对应的滑动平均滤波：
#define SPEED_FILTER_ARRAY_SIZE (40) // 滤波数组大小
static volatile u8 speed_filter_array[SPEED_FILTER_ARRAY_SIZE] = {0};
static volatile u8 speed_filter_index = 0;

// ui处理时速显示的周期
static volatile u8 ui_process_speed_period = 0;
// ui处理发动机转速边框动画的周期
static volatile u16 ui_process_engine_speed_frame_wave_period = 0;

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

void ui_process_engine_speed_frame_wave_1ms_isr(void)
{
    if (ui_process_engine_speed_frame_wave_period < UINT8_T_MAX) {
        ui_process_engine_speed_frame_wave_period++;
    }
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
    static s8 dir = 0;        // 0：向两侧扩散，1：向中间回收
    static s8 spread = -1;    // -1：中间三个灯，0..8：向两侧扩散/回收
    const s8 center_idx = 10; // 中间位置
    const s8 max_spread = 8;  // 扩散到边界位置
    u8 i;

	if (ui_process_engine_speed_frame_wave_period >= 500) {
		ui_process_engine_speed_frame_wave_period = 0;
	}
  

    // 清空当前所有发动机转速边框灯
    for (i = 0; i < ARRAY_SIZE(engine_speed_frame_map); i++) {
        aip3368h_display_engine_speed_frame_by_idx(0, i);
    }

    if (0 == dir) {
        if (spread < 0) {
            // 从中间三个灯开始
            aip3368h_display_engine_speed_frame_by_idx(1, center_idx - 1);
            aip3368h_display_engine_speed_frame_by_idx(1, center_idx);
            aip3368h_display_engine_speed_frame_by_idx(1, center_idx + 1);
            spread = 0;
        } else {
            // 两个灯往左侧移动，两个灯往右侧移动
            aip3368h_display_engine_speed_frame_by_idx(1,
                                                       center_idx - 1 - spread);
            aip3368h_display_engine_speed_frame_by_idx(1,
                                                       center_idx - 2 - spread);
            aip3368h_display_engine_speed_frame_by_idx(1,
                                                       center_idx + 1 + spread);
            aip3368h_display_engine_speed_frame_by_idx(1,
                                                       center_idx + 2 + spread);

            if (spread >= max_spread) {
                dir = 1;
                spread = max_spread - 1;
            } else {
                spread++;
            }
        }
    } else {
        if (spread < 0) {
            // 回收到中间时，仍然保持三个中心灯的状态
            aip3368h_display_engine_speed_frame_by_idx(1, center_idx - 1);
            aip3368h_display_engine_speed_frame_by_idx(1, center_idx);
            aip3368h_display_engine_speed_frame_by_idx(1, center_idx + 1);
            dir = 0;
            spread = 0;
        } else {
            // 碰到边界后，直接开始向中间回收，不再多停一拍
            aip3368h_display_engine_speed_frame_by_idx(1,
                                                       center_idx - 1 - spread);
            aip3368h_display_engine_speed_frame_by_idx(1,
                                                       center_idx - 2 - spread);
            aip3368h_display_engine_speed_frame_by_idx(1,
                                                       center_idx + 1 + spread);
            aip3368h_display_engine_speed_frame_by_idx(1,
                                                       center_idx + 2 + spread);

            if (spread <= 0) {
                dir = 0;
                spread = -1;
            } else {
                spread--;
            }
        }
    }
}
