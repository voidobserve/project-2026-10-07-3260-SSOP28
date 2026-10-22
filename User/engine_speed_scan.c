#include "engine_speed_scan.h"

#include "include.h"
#include "instrument.h"
#include "user_config.h"

#if ENGINE_SPEED_SCAN_ENABLE

volatile u32 engine_speed_scan_cnt = 0; // 当前采样窗口内累计的发动机脉冲数
volatile u16 engine_speed_scan_ms = 0;  // 当前采样窗口已计时长度，单位：ms

// 已完成但尚未处理的窗口脉冲总数
static volatile u32 pending_engine_speed_scan_cnt = 0;
// 待处理完整窗口的累计时长，单位：ms
static volatile u32 pending_engine_speed_scan_ms = 0;
// 距离上次脉冲的时间，单位：ms
static volatile u16 engine_speed_no_pulse_ms = 0;
// 超时已触发，等待新脉冲后解除
static volatile bit engine_speed_timeout_latched = 0;

volatile bit flag_is_engine_speed_scan_over_time =
    0; // 等待主循环处理的超时事件

// 发动机转速的相关配置
void engine_speed_scan_config(void)
{
    P2_MD1 &= ~GPIO_P24_MODE_SEL(0x3); // 输入模式
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

        if (engine_speed_no_pulse_ms < ENGINE_SPEED_SCAN_OVER_TIME) {
            engine_speed_no_pulse_ms++;
            if (engine_speed_no_pulse_ms >= ENGINE_SPEED_SCAN_OVER_TIME &&
                engine_speed_timeout_latched == 0) {
                engine_speed_timeout_latched = 1;
                flag_is_engine_speed_scan_over_time = 1;
            }
        }

        if (engine_speed_scan_ms >= ENGINE_SPEED_SCAN_UPDATE_TIME) {
            pending_engine_speed_scan_cnt += engine_speed_scan_cnt;
            engine_speed_scan_cnt = 0;
            pending_engine_speed_scan_ms += ENGINE_SPEED_SCAN_UPDATE_TIME;
            engine_speed_scan_ms = 0;
        }
    }

    if (ENGINE_SPEED_SCAN_PIN) // 检测发动机转速的引脚
    {
        if (0 == last_engine_speed_scan_level) {
            // 如果之前检测到低电平，现在检测到高电平，说明有上升沿，对脉冲计数加一
            engine_speed_scan_cnt++;
            engine_speed_no_pulse_ms = 0;
            engine_speed_timeout_latched = 0;
            flag_is_engine_speed_scan_over_time = 0;
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
    u32 rpm = 0;
    u32 scan_pulse_snapshot = 0; // 本次消费的窗口脉冲总数
    u32 scan_time_snapshot = 0;  // 本次消费的窗口总时长，单位：ms
    u8 scan_due = 0;             // 是否有完整窗口或待处理超时事件
    u8 is_scan_over_time = 0;    // 本次快照是否包含超时事件

    SYS_ENTER_CRITICAL_SECTION();
    if (pending_engine_speed_scan_ms != 0 ||
        flag_is_engine_speed_scan_over_time) {
        scan_pulse_snapshot = pending_engine_speed_scan_cnt;
        scan_time_snapshot = pending_engine_speed_scan_ms;
        is_scan_over_time = flag_is_engine_speed_scan_over_time;

        pending_engine_speed_scan_cnt = 0;
        pending_engine_speed_scan_ms = 0;
        flag_is_engine_speed_scan_over_time = 0;
        scan_due = 1;
    }
    SYS_EXIT_CRITICAL_SECTION();

    if (scan_due == 0) {
        return;
    }

#if USER_DEBUG_ENABLE
    // printf("engine speed scan time:%lu\n", scan_time_snapshot);
#endif

    if (is_scan_over_time) {
        rpm = 0;
    } else if (scan_time_snapshot != 0) {
        /*
            窗口内转过的圈数 = 单脉冲对应的圈数 * 窗口脉冲数
            每分钟转数 = 窗口内转过的圈数 / 窗口时长 * 60000ms(60000ms == 1min)
        */
        rpm = scan_pulse_snapshot * ENGINE_SPEED_SCAN_A_PULSE_PER_TURNS *
              (CONVER_ONE_MINUTE_TO_MS / ENGINE_SPEED_SCAN_COMPONSATION) /
              scan_time_snapshot;
    }
#if USER_DEBUG_ENABLE
    // printf("engine speed pulse cnt:%lu\n", scan_pulse_snapshot);
#endif

    // 限制得到的发动机转速，避免写入仪表数据时超出16位范围
    if (rpm >= 65535) {
        rpm = 65535;
    }

    instrument.engine_speed = rpm;
#if USER_DEBUG_ENABLE
    // printf("cur rpm %lu\n", rpm);
#endif
}

#endif // #if ENGINE_SPEED_SCAN_ENABLE
