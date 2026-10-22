#ifndef __MILEAGE_H__
#define __MILEAGE_H__

#include "include.h" // 使用芯片官方提供的头文件

// 1m == 1000mm
#define MILE_MM_PER_METER ((u32)1000)
// 每走过多少米，把里程写一次 EEPROM
#define MILE_SAVE_DISTANCE ((u8)100)
// 走过 MILE_SAVE_DISTANCE 之后，至少要再经过多少 ms 才允许写 EEPROM
// （防止里程异常跳变时疯狂写入；正常行驶走完 100m 远大于这个时间）
#define MILE_SAVE_MIN_INTERVAL ((u16)100)
// 车速为 0 持续多少 ms 后，把还没写进 EEPROM 的里程补写一次
#define MILE_STOP_SAVE_TIME ((u16)2000)

/*
	里程每走过 0.1km(100m) 才保存一次，如果不满 0.1km 就掉电，就会丢弃这部分数据
	(擦写寿命限制，不可能每走1m就保存一次)
	停车时（车速为 0 持续 MILE_STOP_SAVE_TIME ）会把剩下的零头补写一次
*/

extern volatile u32 distance; // 存放每次扫描时走过的路程（单位：mm）

/*
	1ms 计时计数，生产者-消费者模型：
	TMR1 中断是生产者（只做累加），主循环是消费者
	（在临界区里一次性取走并清零，见 mileage_scan()）。
	u16 在 8 位机上读取需要多次字节访问，若不加临界区，
	读取过程中被中断打断会读到撕裂的值，所以必须成对使用临界区。
*/
extern volatile u16 mileage_time_1ms_cnt;

void mileage_scan(void); // 里程扫描（大计里程扫描 + 小计里程扫描）

#endif
