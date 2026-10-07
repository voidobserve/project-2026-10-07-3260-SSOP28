#ifndef __SPEED_SCAN_H__
#define __SPEED_SCAN_H__

#include "include.h" // 使用芯片官方提供的头文件
// #include "user_include.h" // 包含自定义的头文件

// ======================================================
// 检测时速所需的配置：
#define SPEED_SCAN_PIN P15 // 检测时速的引脚

/*
    检测到 多少个脉冲 表示 车轮走过一圈（仅供计算时使用，并不代表真实的车轮数据）
    单位：(1 个脉冲 / 10 / SPEED_SCAN_PULSE_COMPENSATION)
*/
#define SPEED_SCAN_PULSE_PER_TURN (1658)
/*
    由于车轮走过一圈的脉冲个数不为整数，这里要加上系数来作补偿
*/
#define SPEED_SCAN_PULSE_COMPENSATION (1000)

// 车轮的一圈对应多少毫米（仅供计算时使用，并不代表真实的车轮数据）
// #define SPEED_SCAN_MM_PER_TURN ((u16)105 * 10) // 一圈 xx 毫米
// #define SPEED_SCAN_MM_PER_TURN ((u16)65 * 10) // 一圈 xx 毫米
#define SPEED_SCAN_MM_PER_TURN ((u16)60 * 10) // 一圈 xx 毫米
// #define SPEED_SCAN_MM_PER_TURN ((u16)55 * 10) // 一圈 xx 毫米

/*
    时速扫描的超时时间，单位：ms
    如果超过这个时间还没有检测到脉冲个数，则认为时速为0

    至少要大于500ms，才能检测到1Hz的信号
*/
#define SPEED_SCAN_OVER_TIME (600) //
// 时速的更新时间，单位：ms
#define SPEED_SCAN_UPDATE_TIME (500)
// #define SPEED_SCAN_UPDATE_TIME (400) // 时间太短，时速会经常跳动（提供1Hz信号，会在0km/h、3km/h这两个值来回切换）

void speed_scan_timer_50us_isr(void);

void speed_scan_config(void);
void speed_scan(void);

#endif
