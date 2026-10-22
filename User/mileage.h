#ifndef __MILEAGE_H__
#define __MILEAGE_H__

#include "include.h" // 使用芯片官方提供的头文件  

/*
	里程每隔0.1km才保存一次，如果不满0.1km就掉电，就会丢弃这部分数据
	(擦写寿命限制，不可能每走1m就保存一次)
*/

// 里程保存时间间隔（单位：ms）
#define MILE_SAVE_INTERVAL (1000)
 

extern volatile u32 distance; // 存放每次扫描时走过的路程
// 里程扫描所需的计数值,每隔一定时间将里程写入flash
extern volatile u16 mileage_save_time_cnt;
// 里程更新的时间计数,每隔一段时间更新一次当前里程（负责控制发送里程的周期）
extern volatile u16 mileage_update_time_cnt;

void mileage_scan(void); // 里程扫描（大计里程扫描 + 小计里程扫描）



#endif