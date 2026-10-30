// 定时器TMR2的驱动头文件：毫秒墙钟时间基准（脉冲采集窗口计时用）
#ifndef __TMR2_H__
#define __TMR2_H__

#include "typedef.h"

void tmr2_config(void);    // 初始化 TMR2 为自由运行时间基准（无中断）
void tmr2_ms_update(void); // TMR1 1ms 中断调用：累计系统毫秒数
u32 tmr2_ms_get(void);     // 读取当前系统毫秒数（主循环调用）

#endif
