#ifndef __UART0_H__
#define __UART0_H__

#include "user_config.h"
#include "typedef.h"

#if USER_DEBUG_ENABLE
// 串口0波特率
#define UART0_BAUDRATE (115200)

void uart0_init(void);
#endif

#endif
