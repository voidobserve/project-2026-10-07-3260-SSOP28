#ifndef __MILEAGE_H__
#define __MILEAGE_H__

#include "include.h" // 使用芯片官方提供的头文件
// #include "user_include.h" // 包含自定义的头文件

// 里程的配置:
// 大小里程的更新时间(单位:ms):
#define MILEAGE_UPDATE_TIME_MS (1000)

extern volatile u32 distance; // 存放每次扫描时走过的路程
extern volatile u16
    mileage_save_time_cnt; // 里程扫描所需的计数值,每隔一定时间将里程写入flash

extern volatile u16
    mileage_update_time_cnt; // 里程更新的时间计数,每隔一段时间更新一次当前里程（负责控制发送里程的周期）

void mileage_scan(void); // 里程扫描（大计里程扫描 + 小计里程扫描）

#endif