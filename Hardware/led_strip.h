/**
 ******************************************************************************
 * @file    led_strip.h
 * @brief   幻彩灯灯带底层驱动（GPIO 位拼发送，逐周期复刻样机波形）
 *
 * 硬件：P26 作为普通 GPIO 输出，两路灯带并联同显，每路 7 颗灯珠。
 * 协议：WS2812 类单线协议，24bit/灯（GRB 顺序），带字节帧结构：
 *   每个灯位 bit：高电平 T0H/T1H + 低电平（字节内约 1.79us）；
 *   每发送完 8 位：低电平延长为字节间隙（5.5~6.3us），灯珠用它做字节同步，
 *   必须严格复现（来源：样机波形实测，间隙位置 3720/3720 全在第 8 位之后）。
 * 时序基准：1T 内核 @48MHz，1 个指令周期 = 20.833ns（数据手册 1.2 特性）。
 *   样机实测 T0H=416.7ns(20 周期)、T1H=708.3ns(34 周期)。
 ******************************************************************************
 */
#ifndef __LED_STRIP_H__
#define __LED_STRIP_H__

#include "include.h"
#include "user_config.h"

/* 驱动引脚（普通 GPIO 输出） */
#define LED_STRIP_PIN P26

/* 每路灯珠数量（来源：样机波形实测，168bit/帧 = 7 灯 × 24bit） */
#define LED_STRIP_LED_NUM ((u8)7)
/* 每颗灯的字节数（24bit GRB） */
#define LED_STRIP_BYTES_PER_LED ((u8)3)
/* 每帧字节数 = 灯数 × 24bit */
#define LED_STRIP_FRAME_LEN ((u8)(LED_STRIP_LED_NUM * LED_STRIP_BYTES_PER_LED))
/* 每字节 bit 数 */
#define LED_STRIP_BITS_PER_BYTE ((u8)8)
/* 逐 bit 取位掩码（MSB 先发） */
#define LED_STRIP_BIT_MSB ((u8)0x80)

/*
 * 时序延时（NOP 数）定义在 led_strip.c 的 LED_STRIP_DELAY_xxx 宏里。
 * 时基：取指 = 系统时钟/2（system.c: FLASH_TIMEREG1=0x54，厂商 48MHz 推荐值），
 *   1 个 NOP = 1 个指令周期 = 41.67ns（与样机波形的 41.67ns 时序粒度一致）。
 * 各延时 = NOP 数 + 实测指令开销（T0H/T1H 路径 2/5 条、低电平路径 29 条、
 * 字节边界 102 条）：
 *   T0H = 8 NOP + 2 = 10 周期 = 416.7ns（样机 416ns）；
 *   T1H = 12 NOP + 5 = 17 周期 = 708.3ns（样机 708ns）；
 *   字节内低电平 = 14 NOP + 29 = 43 周期 = 1.79us（样机 1.58~1.83us）；
 *   字节间隙 = 37 NOP + 102 = 139 周期 = 5.79us（样机 5.2~6.4us，要求 5.5~6.3us）。
 * 校准方法：示波器实测各宽度，偏差 ns 数 / 41.67 = 需增减的 NOP 数，
 * 修改 led_strip.c 对应 LED_STRIP_DELAY_xxx 宏的 NOP 组合。
 */

/*
 * 发送期间是否关闭中断（A/B 测试开关）：
 *   1 = 关中断：时序严格复刻样机（位拼对中断抖动敏感，建议此值）；
 *   0 = 不关中断：时速/转速采样无损，但低电平可能被中断拉长导致灯珠识别错误。
 */
#define LED_STRIP_SEND_MASK_IRQ (1)

void led_strip_config(void);        /* 初始化 P26 为 GPIO 输出 */
void led_strip_show(const u8 *grb); /* 发送一帧，grb 指向 LED_STRIP_FRAME_LEN 字节 */

#endif /* __LED_STRIP_H__ */
