/**
  ******************************************************************************
  * @file    debug.c
  * @author  HUGE-IC Application Team
  * @version V1.0.0
  * @date    01-05-2021
  * @brief   This file contains the debug function.
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

#if 0
/** @addtogroup Template_Project
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

#if DBG_PRINTF_EN
// 重写puchar()函数
char putchar(char c)
{
    while(!(UART1_STA & UART_TX_DONE(0x01)));
    UART1_DATA = c;

    return c;
}
#endif

/**
  * @brief  Debug puchar function
  * @param  uart_data : Data that needs to be sent
  * @retval None
  */
void debug_putchar(u8 uart_data)
{
    while(!(UART1_STA & UART_TX_DONE(0x01)));
    UART1_DATA = uart_data;
    while(!(UART1_STA & UART_TX_DONE(0x01)));
}

/**
  * @brief  Debug initialization function
  * @param  None
  * @retval None
  */
void debug_init(void)
{
    u8 i = 0;
    
    // TX---P30(R16)
    
    P3_MD0   &= ~GPIO_P30_MODE_SEL(0x03); 
    P3_MD0   |=  GPIO_P30_MODE_SEL(0x01); 
    FOUT_S30  =  GPIO_FOUT_UART1_TX;

    UART1_BAUD1 = (((SYSCLK-DEBUG_BAUD)/DEBUG_BAUD) >> 8) & 0xFF;
    UART1_BAUD0 =  ((SYSCLK-DEBUG_BAUD)/DEBUG_BAUD)       & 0xFF;
    UART1_CON0  = UART_EN(0x01);                                    // UART使能
}

/**
  * @brief  Debug User printf function
  * @param  p_data : The string to send
  * @retval None
  */
void user_printf(char *p_data)
{
    while(*p_data != '\0') {
        debug_putchar(*p_data++);
    }
}

#endif

/*************************** (C) COPYRIGHT 2021 HUGE-IC ***** END OF FILE *****/
