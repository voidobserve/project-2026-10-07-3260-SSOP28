#include "speed_scan.h"
#include "instrument.h" // 包含仪表的参数
#include "mileage.h"    // 包含里程的参数 distance
#include "tmr2.h"       // 毫秒墙钟时间基准

#include "user_config.h"

#if SPEED_SCAN_ENABLE

/*
 * 时速脉冲采集：TMR3 外部脉冲计数模式（CAP PIN 上升沿计数，FIN_S3 <- P01）。
 *   脉冲由硬件计数：LED 波形发送关中断期间照常累计，不丢脉冲；
 *   窗口时间取毫秒墙钟：与脉冲口径一致（都是墙钟），速度换算无偏；
 *   窗口到齐才更新 instrument.speed，新数据到来前保持显示。
 */
static u32 pulse_cnt = 0;    // 当前窗口内累计的脉冲数
static u32 win_ms = 0;       // 当前窗口已累计的时长（ms）
static u32 no_pulse_ms = 0;  // 距上次脉冲经过的时间（ms）
static u16 hw_cnt_last = 0;  // 上次读取的硬件脉冲计数值
static u32 last_ms = 0;      // 上次结算时的毫秒时间戳（ms）
static bit timeout_latch = 0; // 超时锁存：已触发过，等新脉冲到来才解除
static bit timeout_flag = 0;  // 超时事件标志：结算时消费

/**
 * @brief 时速扫描的配置：P01 输入 + TMR3 外部脉冲计数
 */
void speed_scan_config(void)
{
    P0_MD0 &= ~GPIO_P01_MODE_SEL(0x03); // 输入模式

    // TMR3：外部脉冲计数（CAP PIN 上升沿），硬件累计，无需中断
    FIN_S3 = GPIO_FIN_SEL_P01;
    TMR_ALLCON = TMR3_CNT_CLR(0x1);    // 清除计数值
    TMR3_PRH = TMR_PERIOD_VAL_H(0xFF); // 周期值最大，自由回绕（差值累计安全）
    TMR3_PRL = TMR_PERIOD_VAL_L(0xFF);
    TMR3_CONH = 0x0;                   // 无中断
    TMR3_CONL = TMR_SOURCE_SEL(0x0) |  // 计数源：GPIO（CAP PIN）上升沿
                TMR_PRESCALE_SEL(0x0) |
                TMR_MODE_SEL(0x01);    // 计数器模式
}

/**
 * @brief 时速扫描：主循环轮询硬件脉冲计数与毫秒时间，窗口到齐结算
 */
void speed_scan(void)
{
    u16 hw_cnt;    // 当前硬件脉冲计数值
    u16 pulse_add; // 本次读取新增的脉冲数
    u32 now_ms;    // 当前毫秒时间戳
    u32 dt_ms;     // 距上次读取经过的时间（ms）
    u32 cur_speed = 0;
    u32 tmp = 0;

    hw_cnt = (u16)(((u16)TMR3_CNTH << 8) | TMR3_CNTL);
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
        if (no_pulse_ms < SPEED_SCAN_OVER_TIME) {
            no_pulse_ms += dt_ms;
            if (no_pulse_ms >= SPEED_SCAN_OVER_TIME && timeout_latch == 0) {
                timeout_latch = 1; // 超时锁存，等新脉冲到来才解除
                timeout_flag = 1;
            }
        }
    }

    // 窗口未到且无超时事件：保持显示，继续累计
    if (win_ms < SPEED_SCAN_UPDATE_TIME && timeout_flag == 0) {
        return;
    }

    /*
            采集到的脉冲个数 / 一圈对应的脉冲个数 * 车轮一圈对应走过的
        距离（单位：mm），计算得到 采集的脉冲个数对应走过的距离（单位：mm）
    */
    tmp = ((pulse_cnt * SPEED_SCAN_MM_PER_TURN) *
           SPEED_SCAN_PULSE_COMPENSATION / SPEED_SCAN_PULSE_PER_TURN);

    if (timeout_flag) {
        // 超时，采集到的脉冲对应一直是0km/h，认为时速是0
        if (pulse_cnt != 0) {
            // 如果采集到的脉冲数不为0
            cur_speed = 1;
        } else {
            cur_speed = 0;
        }
        timeout_flag = 0;
    } else {
        // 未超时，窗口时长换算成以 km/h 为单位的时速（1mm/ms == 1m/s == 3.6km/h）
        cur_speed = tmp * 36 / 10 / win_ms;
    }

    // 防止时速为0时（有可能是推车，记录不到速度），记录不到里程
    distance += tmp; // 累加走过的距离，单位：mm

    pulse_cnt = 0;
    win_ms = 0;

    if (cur_speed > 0) {
        /*
            时速需要与样机对齐（偏差不超过3km/h），
            这里给得到的时速乘以 0.99 作补偿
        */
        cur_speed = (u32)cur_speed * 99 / 100;
        // cur_speed = (u32)cur_speed * 105 / 100;
    }

    // 限制要发送的时速
    if (cur_speed > 199) {
        cur_speed = 199;
    }

    instrument.speed = cur_speed;
}

#endif
