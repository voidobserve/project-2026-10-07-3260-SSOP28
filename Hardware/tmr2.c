// 定时器TMR2的驱动源文件：毫秒墙钟时间基准（脉冲采集窗口计时用）
#include "tmr2.h"
#include "include.h"
#include "user_config.h"

/*
 * TMR2 配置为自由运行时间基准（无中断）：
 *   时钟 = 系统时钟 / 8 = 6MHz，16 位自由回绕（周期约 10.9ms）；
 *   tmr2_ms_update() 在 TMR1 1ms 中断里读取计数差值，累计成系统毫秒数。
 *
 * 计数频率与回绕周期的取舍（读取间隔约 1ms，LED 发送关中断时最大约 1.5ms）：
 *   /8 = 6MHz ：166.7ns/tick，回绕 10.9ms，余量约 7 倍（当前选择）
 *   /2 = 24MHz： 41.7ns/tick，回绕 2.73ms，可用但余量小
 *   /1 = 48MHz：回绕 1.36ms < 读取间隔，会丢时间，不可用
 *
 * 脉冲采集（speed_scan / engine_speed_scan）用本基准计窗口时间：
 * 脉冲由 TMR0/TMR3 硬件计数（LED 发送关中断期间照常累计），
 * 时间用墙钟，两者口径一致，速度/转速换算不受关中断影响。
 */
#define TMR2_TICKS_PER_MS ((u16)6000) // 6MHz 计数 -> 1ms

static volatile u32 sys_ms = 0;    // 系统毫秒数（中断里写，主循环读）
static volatile u16 ms_frac = 0;   // 不足 1ms 的计数余数
static volatile u16 tick_last = 0; // 上次读取的 TMR2 计数值

/**
 * @brief 配置TMR2为自由运行时间基准，无中断
 */
void tmr2_config(void)
{
    TMR_ALLCON = TMR2_CNT_CLR(0x1);        // 清除计数值
    TMR2_PRH = TMR_PERIOD_VAL_H(0xFF);     // 周期值最大，自由回绕
    TMR2_PRL = TMR_PERIOD_VAL_L(0xFF);
    TMR2_CONH = 0x0;                       // 无中断
    TMR2_CONL =
        TMR_SOURCE_SEL(0x06) |   // 时钟源：系统时钟
        TMR_PRESCALE_SEL(0x03) | // 8 分频：6MHz
        TMR_MODE_SEL(0x01);      // 计数器模式
    tick_last = (u16)(((u16)TMR2_CNTH << 8) | TMR2_CNTL);
}

/**
 * @brief 累计系统毫秒数（TMR1 1ms 中断内调用，16 位计数回绕安全）
 */
void tmr2_ms_update(void)
{
    u16 tick = (u16)(((u16)TMR2_CNTH << 8) | TMR2_CNTL);

    ms_frac += (u16)(tick - tick_last); // 差值累计，回绕安全
    tick_last = tick;
    while (ms_frac >= TMR2_TICKS_PER_MS) {
        ms_frac -= TMR2_TICKS_PER_MS;
        sys_ms++;
    }
}

/**
 * @brief 读取当前系统毫秒数
 */
u32 tmr2_ms_get(void)
{
    u32 ms;

    SYS_ENTER_CRITICAL_SECTION(); // sys_ms 为 32 位，读取需防止被中断打断
    ms = sys_ms;
    SYS_EXIT_CRITICAL_SECTION();
    return ms;
}
