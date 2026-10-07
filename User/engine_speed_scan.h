#ifndef __ENGINE_SPEED_SCAN_H__
#define __ENGINE_SPEED_SCAN_H__

#include "typedef.h"
#include "user_config.h" // 包含自定义的头文件

#if ENGINE_SPEED_SCAN_ENABLE

// ======================================================
// 检测发动机转速的引脚
#define ENGINE_SPEED_SCAN_PIN P02

// ============================================================
//
// 检测到一个脉冲表示发动机转了 xx 圈，单位：0.001圈
// #define ENGINE_SPEED_SCAN_A_PULSE_PER_TURNS ((u32)1110)
#define ENGINE_SPEED_SCAN_A_PULSE_PER_TURNS ((u32)1057)
// 补偿系数，根据上面的宏对应的单位，需要利用该系数，恢复成以1圈为单位的值
#define ENGINE_SPEED_SCAN_COMPONSATION ((u32)1000)

// 发动机转速的更新时间，单位：ms
#define ENGINE_SPEED_SCAN_UPDATE_TIME (500)
/*
    发动机转速的超时时间，单位：ms
    如果超时时间到来，还没有检测到脉冲，认为发动机转速为0
*/
#define ENGINE_SPEED_SCAN_OVER_TIME (600)

// 检测发动机转速所需的配置
// ======================================================
// 标志位，发动机转速检测是否超时
extern volatile bit flag_is_engine_speed_scan_over_time;
extern volatile u32 engine_speed_scan_cnt; // 检测到的脉冲个数，在定时器中断累加
extern volatile u16 engine_speed_scan_ms;

extern void engine_speed_scan_config(void); // 发动机转速扫描的配置
extern void engine_speed_scan(void);        // 发动机转速扫描

void engine_speed_scan_timer_50us_isr(void);

#endif // #if ENGINE_SPEED_SCAN_ENABLE

#endif