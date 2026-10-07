// 定时器TMR2的驱动源文件
#include "tmr2.h"
#include "include.h"
#include "user_config.h"

#include "speed_scan.h"
#include "engine_speed_scan.h"

// 定时器定时周期 (单位:Hz)
// 周期值 = 系统时钟 / 定时器分频 / 频率 - 1
#define TMR2_PERIOD (SYSCLK / 1 / 20000 - 1) // 20khz，50us
/**
 * @brief 配置定时器TMR2，配置完成后，定时器默认关闭
 */
void tmr2_config(void)
{
    __SetIRQnIP(TMR2_IRQn, TMR2_IQn_CFG); // 设置中断优先级
    __DisableIRQ(TMR2_IRQn);              // 禁用中断
    IE_EA = 1;                            // 打开总中断

    // 清除TMR2的计数值
    TMR_ALLCON = TMR2_CNT_CLR(0x1); // 清除计数值

    // 配置TMR2的计数周期
    TMR2_PRH = TMR_PERIOD_VAL_H((TMR2_PERIOD >> 8) & 0xFF); // 周期值
    TMR2_PRL = TMR_PERIOD_VAL_L((TMR2_PERIOD >> 0) & 0xFF);

    TMR2_CONH &= ~TMR_PRD_PND(0x01); // 清除TMR2的计数标志位，表示未完成计数
    TMR2_CONH |= TMR_PRD_IRQ_EN(1);  // 使能TMR2的计数中断

    TMR2_CONL &= ~(TMR_SOURCE_SEL(0x07) |   // 清除定时器的时钟源配置寄存器
                   TMR_PRESCALE_SEL(0x07) | // 清除TMR2的预分频配置寄存器
                   TMR_MODE_SEL(0x03));     // 清除TMR2的模式配置寄存器
    TMR2_CONL |=
        TMR_SOURCE_SEL(0x06) |   // 配置定时器的时钟源，使用系统时钟
        TMR_PRESCALE_SEL(0x00) | // 定时器预分频
        TMR_MODE_SEL(
            0x01); // 配置TMR2的模式为计数器模式，最后对系统时钟的脉冲进行计数

    __EnableIRQ(TMR2_IRQn); // 使能中断
}

// TMR2中断服务函数
void TIMR2_IRQHandler(void) interrupt TMR2_IRQn
{
    // 进入中断设置IP，不可删除
    __IRQnIPnPush(TMR2_IRQn);

    // ---------------- 用户函数处理 -------------------
    // 周期中断
    if (TMR2_CONH & TMR_PRD_PND(0x1)) {
        TMR2_CONH |= TMR_PRD_PND(0x1); // 清除pending

#if ENGINE_SPEED_SCAN_ENABLE
        engine_speed_scan_timer_50us_isr();
#endif

#if SPEED_SCAN_ENABLE
        speed_scan_timer_50us_isr();
#endif
    }

    // 退出中断设置IP，不可删除
    __IRQnIPnPop(TMR2_IRQn);
}
