/**
 ******************************************************************************
 * @file    led_anim.c
 * @brief   幻彩灯动画状态机（上电直接按车速运行，无开机动画）
 ******************************************************************************
 */
#include "led_anim.h"

#if LED_ANIM_ENABLE

#include "led_strip.h"
#include "instrument.h"

/* 波浪循环：每帧点亮前 N 颗灯（来源：样机实测 14 帧循环） */
static const u8 wave_seq[LED_ANIM_WAVE_SEQ_LEN] = {1, 1, 1, 2, 3, 4, 5,
                                                   6, 7, 6, 5, 4, 3, 2};

/* 波浪模式灯位颜色（GRB 满亮，来源：样机实测 绿绿青青黄黄红） */
static const u8 wave_led_color[LED_STRIP_LED_NUM][LED_STRIP_BYTES_PER_LED] = {
    {0xFF, 0x00, 0x00}, /* 绿 */
    {0xFF, 0x00, 0x00}, /* 绿 */
    {0xFF, 0x00, 0xFF}, /* 青 */
    {0xFF, 0x00, 0xFF}, /* 青 */
    {0xFF, 0xFF, 0x00}, /* 黄 */
    {0xFF, 0xFF, 0x00}, /* 黄 */
    {0x00, 0xFF, 0x00}, /* 红 */
};

/* 五色呼吸颜色通道开关（GRB，1=该通道随亮度呼吸）：绿->蓝->青->紫->白
 *（来源：样机实测） */
static const u8 breath_color_mask[LED_ANIM_COLOR_NUM][LED_STRIP_BYTES_PER_LED] = {
    {1, 0, 0}, /* 绿 */
    {0, 0, 1}, /* 蓝 */
    {1, 0, 1}, /* 青 */
    {0, 1, 1}, /* 紫 */
    {1, 1, 1}, /* 白 */
};

/* 超速红色呼吸颜色通道开关（R） */
static const u8 breath_red_mask[LED_STRIP_BYTES_PER_LED] = {0, 1, 0};

/* 1ms 计数（TMR1 中断累加，主循环取走） */
static volatile u16 led_anim_ms_cnt = 0;

/* 动画状态 */
static u8 cur_mode = LED_ANIM_MODE_WAVE;
static u8 pend_mode = LED_ANIM_MODE_WAVE;
static u16 mode_delay_ms = 0;
static u16 frame_acc_x10 = 0; /* 呼吸帧调度累加器（0.1ms） */
static u16 wave_acc_ms = 0;   /* 波浪帧调度累加器（ms） */
static u8 wave_idx = 0;
static u8 color_idx = 0;
static u8 breath_cnt = 0;
static u8 bright = LED_ANIM_BRIGHT_MIN;
static u8 bright_dir = 0; /* 1=渐亮，0=渐暗 */

/**
 * @brief  由屏显时速得出目标动画模式
 */
static u8 led_anim_mode_from_speed(u8 speed)
{
    if (speed == 0) {
        return LED_ANIM_MODE_WAVE;
    }
    if (speed >= LED_ANIM_SPEED_RED_THRESHOLD) {
        return LED_ANIM_MODE_BREATH_RED;
    }
    return LED_ANIM_MODE_BREATH_COLOR;
}

/**
 * @brief  动画状态复位（模式切换后从头开始播放）
 */
static void led_anim_reset(void)
{
    frame_acc_x10 = 0;
    wave_acc_ms = 0;
    wave_idx = 0;
    color_idx = 0;
    breath_cnt = 0;
    bright = LED_ANIM_BRIGHT_MIN;
    bright_dir = 1;
}

/**
 * @brief  动画模式判定与切换（带切换延迟，防止边界跳动频繁切换）
 * @param  elapsed_ms 距上次调用经过的毫秒数
 */
static void led_anim_update_mode(u16 elapsed_ms)
{
    u8 want = led_anim_mode_from_speed(instrument.speed_to_display);

    if (want == cur_mode) {
        pend_mode = cur_mode;
        mode_delay_ms = 0;
        return;
    }
    if (want != pend_mode) {
        pend_mode = want;
        mode_delay_ms = 0;
        return;
    }
    mode_delay_ms += elapsed_ms;
    if (mode_delay_ms >= LED_ANIM_MODE_SWITCH_DELAY_MS) {
        cur_mode = pend_mode;
        led_anim_reset();
    }
}

/**
 * @brief  发送一帧波浪动画并推进波浪相位
 */
static void led_anim_send_wave_frame(void)
{
    u8 frame[LED_STRIP_FRAME_LEN];
    u8 lit = wave_seq[wave_idx];
    u8 i;

    for (i = 0; i < LED_STRIP_LED_NUM; i++) {
        if (i < lit) {
            frame[i * LED_STRIP_BYTES_PER_LED] = wave_led_color[i][0];
            frame[i * LED_STRIP_BYTES_PER_LED + 1] = wave_led_color[i][1];
            frame[i * LED_STRIP_BYTES_PER_LED + 2] = wave_led_color[i][2];
        } else {
            frame[i * LED_STRIP_BYTES_PER_LED] = 0;
            frame[i * LED_STRIP_BYTES_PER_LED + 1] = 0;
            frame[i * LED_STRIP_BYTES_PER_LED + 2] = 0;
        }
    }
    led_strip_show(frame);

    wave_idx++;
    if (wave_idx >= LED_ANIM_WAVE_SEQ_LEN) {
        wave_idx = 0;
    }
}

/**
 * @brief  发送一帧呼吸动画（整条灯带同色，激活通道取当前亮度）
 * @param  mask 颜色通道开关（GRB）
 */
static void led_anim_send_breath_frame(const u8 *mask)
{
    u8 frame[LED_STRIP_FRAME_LEN];
    u8 i;

    for (i = 0; i < LED_STRIP_LED_NUM; i++) {
        frame[i * LED_STRIP_BYTES_PER_LED] = mask[0] ? bright : 0;
        frame[i * LED_STRIP_BYTES_PER_LED + 1] = mask[1] ? bright : 0;
        frame[i * LED_STRIP_BYTES_PER_LED + 2] = mask[2] ? bright : 0;
    }
    led_strip_show(frame);
}

/**
 * @brief  呼吸亮度三角波推进一步（每帧调用一次）
 */
static void led_anim_breath_step(void)
{
    if (bright_dir) {
        bright++;
        if (bright >= LED_ANIM_BRIGHT_MAX) {
            bright = LED_ANIM_BRIGHT_MAX;
            bright_dir = 0;
        }
    } else {
        bright--;
        if (bright <= LED_ANIM_BRIGHT_MIN) {
            bright = LED_ANIM_BRIGHT_MIN;
            bright_dir = 1;
            breath_cnt++;
            if (breath_cnt >= LED_ANIM_BREATH_PER_COLOR) {
                breath_cnt = 0;
                color_idx++;
                if (color_idx >= LED_ANIM_COLOR_NUM) {
                    color_idx = 0;
                }
            }
        }
    }
}

void led_anim_config(void)
{
    led_strip_config();
    led_anim_reset();
    cur_mode = led_anim_mode_from_speed(instrument.speed_to_display);
    pend_mode = cur_mode;
    mode_delay_ms = 0;
}

void led_anim_timer_1ms_isr(void)
{
    if (led_anim_ms_cnt < LED_ANIM_MS_CNT_MAX) {
        led_anim_ms_cnt++;
    }
}

void led_anim_handle(void)
{
    u16 elapsed;
    u16 ms;

    SYS_ENTER_CRITICAL_SECTION();
    elapsed = led_anim_ms_cnt;
    led_anim_ms_cnt = 0;
    SYS_EXIT_CRITICAL_SECTION();

    if (elapsed == 0) {
        return;
    }

    led_anim_update_mode(elapsed);

    if (LED_ANIM_MODE_WAVE == cur_mode) {
        wave_acc_ms += elapsed;
        while (wave_acc_ms >= LED_ANIM_WAVE_FRAME_MS) {
            wave_acc_ms -= LED_ANIM_WAVE_FRAME_MS;
            led_anim_send_wave_frame();
        }
    } else {
        ms = elapsed;
        while (ms > 0) {
            ms--;
            frame_acc_x10 += LED_ANIM_TENTHS_PER_MS;
            while (frame_acc_x10 >= LED_ANIM_BREATH_FRAME_X10) {
                frame_acc_x10 -= LED_ANIM_BREATH_FRAME_X10;
                led_anim_breath_step();
                if (LED_ANIM_MODE_BREATH_RED == cur_mode) {
                    led_anim_send_breath_frame(breath_red_mask);
                } else {
                    led_anim_send_breath_frame(breath_color_mask[color_idx]);
                }
            }
        }
    }
}

#endif /* LED_ANIM_ENABLE */
