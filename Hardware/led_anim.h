/**
 ******************************************************************************
 * @file    led_anim.h
 * @brief   幻彩灯动画状态机（上电直接按车速运行，无开机动画）
 *
 * 动画判定依据 instrument.speed_to_display（屏显时速）：
 *   == 0            ：无时速，色带波浪填充动画
 *   1 ~ 99          ：五色呼吸动画（绿->蓝->青->紫->白）
 *   >= 100          ：红色呼吸动画（超速）
 * 模式切换带延迟，防止时速在阈值边界跳动时动画频繁切换。
 * 全部动画参数来源：样机波形实测（见《样机幻彩灯驱动信号与动画解析.md》）。
 ******************************************************************************
 */
#ifndef __LED_ANIM_H__
#define __LED_ANIM_H__

#include "include.h"
#include "user_config.h"

#if LED_ANIM_ENABLE

/* 动画模式 */
enum {
    LED_ANIM_MODE_WAVE = 0,     /* 无时速：色带波浪填充 */
    LED_ANIM_MODE_BREATH_COLOR, /* 时速 1~99：五色呼吸 */
    LED_ANIM_MODE_BREATH_RED,   /* 时速 >= 100：红色呼吸 */
};

/* 超速红色动画阈值（km/h，来源：需求"大于等于 100 用超过 100 的动画"） */
#define LED_ANIM_SPEED_RED_THRESHOLD ((u8)100)

/* 动画模式切换延迟（ms），防止边界跳动频繁切换（需求给定范围 1~2s，取 1.5s） */
#define LED_ANIM_MODE_SWITCH_DELAY_MS ((u16)1500)

/* 呼吸动画帧周期：6.6ms/帧（来源：样机实测），以 0.1ms 累加实现，无累积误差 */
#define LED_ANIM_BREATH_FRAME_X10 ((u16)66)
/* 每毫秒的 0.1ms 计数 */
#define LED_ANIM_TENTHS_PER_MS ((u16)10)

/* 波浪动画帧周期：205ms/帧（来源：样机实测） */
#define LED_ANIM_WAVE_FRAME_MS ((u16)205)
/* 波浪循环长度（帧，来源：样机实测 14 帧循环） */
#define LED_ANIM_WAVE_SEQ_LEN ((u8)14)

/* 呼吸亮度范围（来源：样机实测 6~254），每帧步进 1 级 */
#define LED_ANIM_BRIGHT_MIN ((u8)6)
#define LED_ANIM_BRIGHT_MAX ((u8)254)
/* 每种颜色连续呼吸次数（来源：样机实测 3 次，约 9.9s） */
#define LED_ANIM_BREATH_PER_COLOR ((u8)3)
/* 五色呼吸颜色数（绿->蓝->青->紫->白） */
#define LED_ANIM_COLOR_NUM ((u8)5)

/* 1ms 计数饱和上限 */
#define LED_ANIM_MS_CNT_MAX ((u16)65535)

void led_anim_config(void);           /* 初始化（含 led_strip_config），上电即按当前车速运行动画 */
void led_anim_timer_1ms_isr(void);    /* TMR1 1ms 中断内调用，仅做计数 */
void led_anim_handle(void);           /* 主循环调用：模式判定、帧调度与发送 */

#endif /* LED_ANIM_ENABLE */

#endif /* __LED_ANIM_H__ */
