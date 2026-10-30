/**
 ******************************************************************************
 * @file    led_strip.c
 * @brief   幻彩灯灯带底层驱动（GPIO 位拼发送，逐周期复刻样机波形）
 ******************************************************************************
 */
#include "led_strip.h"

#include <intrins.h>

/* NOP 组合宏（1 个 _nop_() = 1 个指令周期 = 20.833ns @1T/48MHz） */
#define LED_STRIP_NOP1() _nop_()
#define LED_STRIP_NOP2()                                                       \
    LED_STRIP_NOP1();                                                          \
    LED_STRIP_NOP1()
#define LED_STRIP_NOP4()                                                       \
    LED_STRIP_NOP2();                                                          \
    LED_STRIP_NOP2()
#define LED_STRIP_NOP8()                                                       \
    LED_STRIP_NOP4();                                                          \
    LED_STRIP_NOP4()
#define LED_STRIP_NOP16()                                                      \
    LED_STRIP_NOP8();                                                          \
    LED_STRIP_NOP8()
#define LED_STRIP_NOP32()                                                      \
    LED_STRIP_NOP16();                                                         \
    LED_STRIP_NOP16()
#define LED_STRIP_NOP64()                                                      \
    LED_STRIP_NOP32();                                                         \
    LED_STRIP_NOP32()
#define LED_STRIP_NOP128()                                                     \
    LED_STRIP_NOP64();                                                         \
    LED_STRIP_NOP64()
#define LED_STRIP_NOP256()                                                     \
    LED_STRIP_NOP128();                                                        \
    LED_STRIP_NOP128()

/*
 * 定长延时：由 LED_STRIP_xxx_NOPS 个 NOP 组成
 *（组合式展开保证周期精确，校准时按注释增减 NOP）
 * 计数依据：取指=系统时钟/2（FLASH_TIMEREG1=0x54）后 1 个 NOP = 41.67ns，
 * 实测开销：T0H/T1H 路径约 2/5 条指令、字节内低电平路径约 29 条、字节边界约 102 条。
 */
#define LED_STRIP_DELAY_T0H()                                                  \
    do {                                                                       \
        LED_STRIP_NOP8(); /* 8 + 开销 2 = 10 周期 = 416.7ns */                 \
    } while (0)
#define LED_STRIP_DELAY_T1H()                                                  \
    do {                                                                       \
        LED_STRIP_NOP8();                                                      \
        LED_STRIP_NOP4(); /* 12 + 开销 5 = 17 周期 = 708.3ns */                \
    } while (0)
#define LED_STRIP_DELAY_BIT_LOW()                                              \
    do {                                                                       \
        LED_STRIP_NOP8();                                                      \
        LED_STRIP_NOP4();                                                      \
        LED_STRIP_NOP2(); /* 14 + 29 = 1.79us */                               \
    } while (0)
#define LED_STRIP_DELAY_BYTE_GAP()                                             \
    do {                                                                       \
        LED_STRIP_NOP32();                                                     \
        LED_STRIP_NOP4();                                                      \
        LED_STRIP_NOP1(); /* 37 + 102 = 5.79us */                              \
    } while (0)

/**
 * @brief  初始化 P26 为 GPIO 输出（空闲低电平）
 */
void led_strip_config(void)
{
    P2_MD1 &= ~GPIO_P26_MODE_SEL(0x03);
    P2_MD1 |= GPIO_P26_MODE_SEL(0x01); /* 输出模式 */
    FOUT_S26 = GPIO_FOUT_AF_FUNC;      /* 普通 GPIO 输出 */
    LED_STRIP_PIN = 0;
}

/**
 * @brief  发送一帧灯带数据（位拼，逐周期复刻样机波形）
 * @param  grb 指向 LED_STRIP_FRAME_LEN 字节的 GRB 数据（每灯 G、R、B 各 1 字节）
 * @note   波形结构与样机一致：
 *           位：高 T0H/T1H -> 低（字节内约 1.79us）；
 *           每 8 位后低电平延长为字节间隙（约 5.8us，灯珠字节同步用）；
 *         发送期间关中断见 LED_STRIP_SEND_MASK_IRQ（位拼时序对中断敏感）。
 */
void led_strip_show(const u8 *grb)
{
    u8 i;
    u8 j;
    u8 dat;

#if LED_STRIP_SEND_MASK_IRQ
    SYS_ENTER_CRITICAL_SECTION();
#endif

    for (i = 0; i < LED_STRIP_FRAME_LEN; i++) {
        dat = *grb;
        grb++;
        for (j = 0; j < LED_STRIP_BITS_PER_BYTE; j++) {
            if (dat & LED_STRIP_BIT_MSB) {
                LED_STRIP_PIN = 1;
                LED_STRIP_DELAY_T1H();
                LED_STRIP_PIN = 0;
            } else {
                LED_STRIP_PIN = 1;
                LED_STRIP_DELAY_T0H();
                LED_STRIP_PIN = 0;
            }
            dat <<= 1;
            if (j == (LED_STRIP_BITS_PER_BYTE - 1)) {
                /* 第 8 位后：字节间隙（灯珠字节同步） */
                LED_STRIP_DELAY_BYTE_GAP();
            } else {
                LED_STRIP_DELAY_BIT_LOW();
            }
        }
    }

#if LED_STRIP_SEND_MASK_IRQ
    SYS_EXIT_CRITICAL_SECTION();
#endif
}
