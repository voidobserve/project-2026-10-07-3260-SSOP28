#ifndef __AIP3368_DRIVER_H__
#define __AIP3368_DRIVER_H__

#include "typedef.h"
 

/*
	有多少个芯片级联
*/
#define AIP3368H_IC_NUM (13)

/*
	串行数据输入端 
*/
#define DIO P13
#define DCK P14 // 串行时钟信号的输入端
#define LAT P30 // 数据锁存
#define PDM -1  // 输出使能控制端口（未使用）

extern volatile u16 aip3368h_display_buff[AIP3368H_IC_NUM];

// 延时函数，根据需要决定使用，测试48Mhz主频无定时中断不需要延时也能正常点亮
#define aip3368h_delay()

void aip3368h_refresh_time_add();

void aip3368h_module_init(void);
void aip3368h_module_display(void);

#endif