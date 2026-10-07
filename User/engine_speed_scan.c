#include "engine_speed_scan.h"

#include "include.h"
#include "instrument.h"
#include "user_config.h"

#if ENGINE_SPEED_SCAN_ENABLE

volatile u32 engine_speed_scan_cnt; // 检测到的脉冲个数，在定时器中断累加
volatile u16 engine_speed_scan_ms;  // 在定时器中断累加

static volatile u32 cur_engine_speed_scan_cnt;
static volatile u32 cur_engine_speed_scan_ms;

volatile bit flag_is_engine_speed_scan_over_time; // 标志位，检测是否超时

// 发动机转速的相关配置
void engine_speed_scan_config(void)
{
    P0_MD0 &= ~GPIO_P02_MODE_SEL(0x3); // 输入模式
}

void update_engine_speed_scan_data(void) // 更新检测发动机转速的数据
{
    cur_engine_speed_scan_cnt += engine_speed_scan_cnt;
    engine_speed_scan_cnt = 0;
    cur_engine_speed_scan_ms += engine_speed_scan_ms;
    engine_speed_scan_ms = 0;
}

void engine_speed_scan_timer_50us_isr(void)
{
    // 记录上一次检测到的引脚电平（发送机转速检测脚）
    static volatile bit last_engine_speed_scan_level = 0;
    // 记录发动机转速扫描的时间
    static u8 cnt = 0;

    cnt++;
    if (cnt >= 20) // 每1ms进入一次
    {
        cnt = 0;
        engine_speed_scan_ms++;

        if (engine_speed_scan_ms >= ENGINE_SPEED_SCAN_OVER_TIME &&
            flag_is_engine_speed_scan_over_time == 0) {
            engine_speed_scan_ms = 0;
            // 说明超时，脉冲计数一直没有加一
            flag_is_engine_speed_scan_over_time = 1;
        }
    }

    if (ENGINE_SPEED_SCAN_PIN) // 检测发动机转速的引脚
    {
        if (0 == last_engine_speed_scan_level) {
            // 如果之前检测到低电平，现在检测到高电平，说明有上升沿，对脉冲计数加一
            engine_speed_scan_cnt++;
            update_engine_speed_scan_data();
        }

        last_engine_speed_scan_level = 1;
    } else {
        // 如果现在检测到低电平
        last_engine_speed_scan_level = 0;
    }
}

// 发动机转速扫描
void engine_speed_scan(void)
{
#define CONVER_ONE_MINUTE_TO_MS (60000UL) // 将1min转换成以ms为单位的数据
    // 初始化为 0，防止后续比较时使用未初始化的值
    volatile u32 rpm = 0;

    if (cur_engine_speed_scan_ms >= ENGINE_SPEED_SCAN_UPDATE_TIME ||
        flag_is_engine_speed_scan_over_time) {
#if USER_DEBUG_ENABLE
// printf("cur_engine_speed_scan_ms:%lu\n", cur_engine_speed_scan_ms);
#endif
        if (flag_is_engine_speed_scan_over_time) {
            flag_is_engine_speed_scan_over_time = 0;
            rpm = 0;
        } else {
            /*
                扫描时间内转过的圈数 == 一个脉冲对应转过的圈数 * 扫描时间内采集到的脉冲个数
                1min转过的圈数 == 扫描时间内转过的圈数 / 扫描时间 * 1min
            */
            rpm = (u32)cur_engine_speed_scan_cnt *
                  ENGINE_SPEED_SCAN_A_PULSE_PER_TURNS *
                  (CONVER_ONE_MINUTE_TO_MS / ENGINE_SPEED_SCAN_COMPONSATION) /
                  cur_engine_speed_scan_ms;
        }
#if USER_DEBUG_ENABLE
        // 打印检测到的脉冲个数
        // printf("cur engine speed pulse cnt:%lu\n", cur_engine_speed_scan_cnt);
#endif

        cur_engine_speed_scan_cnt = 0;
        cur_engine_speed_scan_ms = 0;

        // 限制得到的发动机转速
        if (rpm >= 65535) {
            rpm = 65535;
        }

        instrument.engine_speed = rpm;
#if USER_DEBUG_ENABLE
        // printf("cur rpm %lu\n", rpm);
#endif
    }
}

#endif // #if ENGINE_SPEED_SCAN_ENABLE
