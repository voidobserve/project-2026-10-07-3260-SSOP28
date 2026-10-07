/**
 ******************************************************************************
 * @file    system.c
 * @author  HUGE-IC Application Team
 * @version V1.0.0
 * @date    01-05-2021
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2021 HUGE-IC</center></h2>
 *
 *
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "include.h"

/** @addtogroup Template_Project
 * @{
 */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
 * @brief  Delay function
 * @param  cnt : the number of cnt to delay
 * @retval None
 */
void delay(u16 cnt)
{
  while (cnt--)
  {
  }
}

/**
 * @brief  Delay function
 * @param  ms : the number of ms to delay
 * @retval None
 */
void delay_ms(u16 ms)
{
  while (ms--)
  {
    delay(1450);
    WDT_KEY = WDT_KEY_VAL(0xAA); // 喂狗并清除 wdt_pending
  }
}

/**
 * @brief  System initialization function
 * @param  None
 * @retval None
 */
void system_init(void)
{
  u8 i;

  CLK_ACON0 |= CLK_AIP_HRC_EN(0x1); // 使能 HIRC
  __HIRC_DELAY;                     // 该延迟不可删除,等待HIRC稳定并能保证烧录稳定性
  FLASH_TIMEREG0 = 0x55;
  /* 如果系统时钟配置为48mhz，则FLASH_TIMEREG1 = 0x54 */
  /* 如果系统时钟配置为24mhz-12mhz，则FLASH_TIMEREG1 = 0x50 */
  FLASH_TIMEREG1 = 0x5C;              // FLASH访问速度 = 系统时钟/4
  CLK_CON2 = 0x50 | CLK_SYS_DIV(0x0); // 系统时钟不分频 HIRC48M
  CLK_CON0 = CLK_SYSCLK_SEL(0x3);     // 系统时钟选择
  CLK_CON6 = CLK_FLASH_DIV(47);       // FLASH烧写时钟48分频：1M
  LVD_CON2 = 3;                       // 滤波周期
  LVD_CON1 &= ~(0x3);                 // VCC\VDD 低电滤波去抖功能

  // 初始化寄存器
  // FOUT_S00 ~ FOUT_SEL
  for (i = 0x18; i <= 0x32; i++)
  {
    *(unsigned char volatile xdata *)(IOFUNC_BASE + i) = 0x0;
  }

  __LOADER_CHIP_TRIM; // 装载芯片各个模块校准值
}

/*************************** (C) COPYRIGHT 2021 HUGE-IC ***** END OF FILE *****/
