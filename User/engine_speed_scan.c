#include "engine_speed_scan.h"

#include "include.h"
#include "instrument.h"
#include "tmr2.h" // 毫秒墙钟时间基准
#include "user_config.h"

#if ENGINE_SPEED_SCAN_ENABLE

#define CONVER_ONE_MINUTE_TO_MS (60000UL) // 将 min 转换成以 ms 为单位的数据

/*
 * 发动机转速脉冲采集：TMR0 外部脉冲计数模式（CAP PIN 上升沿计数，FIN_S0 <- P24）。
 *   脉冲由硬件计数：LED 波形发送关中断期间照常累计，不丢脉冲；
 *   窗口时间取毫秒墙钟：与脉冲口径一致（都是墙钟），转速换算无偏；
 *   窗口到齐才更新 instrument.engine_speed，新数据到来前保持显示。
 */
static u32 pulse_cnt = 0;    // 当前窗口内累计的脉冲数
static u32 win_ms = 0;       // 当前窗口已累计的时长（ms）
static u32 no_pulse_ms = 0;  // 距上次脉冲经过的时间（ms）
static u16 hw_cnt_last = 0;  // 上次读取的硬件脉冲计数值
static u32 last_ms = 0;      // 上次结算时的毫秒时间戳（ms）
static bit timeout_latch = 0; // 超时锁存：已触发过，等新脉冲到来才解除
static bit timeout_flag = 0;  // 超时事件标志：结算时消费

/**
 * @brief 发动机转速扫描的配置：P24 输入 + TMR0 外部脉冲计数
 */
void engine_speed_scan_config(void)
{
    P2_MD1 &= ~GPIO_P24_MODE_SEL(0x3); // 输入模式

    // TMR0：外部脉冲计数（CAP PIN 上升沿），硬件累计，无需中断
    FIN_S0 = GPIO_FIN_SEL_P24;
    TMR_ALLCON = TMR0_CNT_CLR(0x1);    // 清除计数值
    TMR0_PRH = TMR_PERIOD_VAL_H(0xFF); // 周期值最大，自由回绕（差值累计安全）
    TMR0_PRL = TMR_PERIOD_VAL_L(0xFF);
    TMR0_CONH = 0x0;                   // 无中断
    TMR0_CONL = TMR_SOURCE_SEL(0x0) |  // 计数源：GPIO（CAP PIN）上升沿
                TMR_PRESCALE_SEL(0x0) |
                TMR_MODE_SEL(0x01);    // 计数器模式
}

/**
 * @brief 发动机转速扫描：主循环轮询硬件脉冲计数与毫秒时间，窗口到齐结算
 */
void engine_speed_scan(void)
{
    u16 hw_cnt;    // 当前硬件脉冲计数值
    u16 pulse_add; // 本次读取新增的脉冲数
    u32 now_ms;    // 当前毫秒时间戳
    u32 dt_ms;     // 距上次读取经过的时间（ms）
    u32 rpm = 0;

    hw_cnt = (u16)(((u16)TMR0_CNTH << 8) | TMR0_CNTL);
    pulse_add = (u16)(hw_cnt - hw_cnt_last); // 16 位回绕安全
    hw_cnt_last = hw_cnt;

    now_ms = tmr2_ms_get();
    dt_ms = now_ms - last_ms;
    last_ms = now_ms;
    win_ms += dt_ms;

    if (pulse_add != 0) {
        pulse_cnt += pulse_add;
        no_pulse_ms = 0;
        timeout_latch = 0;
        timeout_flag = 0;
    } else {
        if (no_pulse_ms < ENGINE_SPEED_SCAN_OVER_TIME) {
            no_pulse_ms += dt_ms;
            if (no_pulse_ms >= ENGINE_SPEED_SCAN_OVER_TIME &&
                timeout_latch == 0) {
                timeout_latch = 1; // 超时锁存，等新脉冲到来才解除
                timeout_flag = 1;
            }
        }
    }

    // 窗口未到且无超时事件：保持显示，继续累计
    if (win_ms < ENGINE_SPEED_SCAN_UPDATE_TIME && timeout_flag == 0) {
        return;
    }

    if (timeout_flag) {
        rpm = 0;
        timeout_flag = 0;
    } else {
        /*
            窗口内转过的圈数 = 单脉冲对应的圈数 * 窗口脉冲数
            每分钟转数 = 窗口内转过的圈数 / 窗口时长 * 60000ms(60000ms == 1min)
        */
        rpm = pulse_cnt * ENGINE_SPEED_SCAN_A_PULSE_PER_TURNS *
              (CONVER_ONE_MINUTE_TO_MS / ENGINE_SPEED_SCAN_COMPONSATION) /
              win_ms;
    }

    // 限制得到的发动机转速，避免写入仪表数据时超出 16 位范围
    if (rpm >= 65535) {
        rpm = 65535;
    }

    pulse_cnt = 0;
    win_ms = 0;

    instrument.engine_speed = rpm;
}

#endif // #if ENGINE_SPEED_SCAN_ENABLE
