/**
  ******************************************************************************
  * @file    User/tk_debug.c
  * @author  TAIXIN-IC Application Team
  * @version V1.0.0
  * @date    05-20-2022
  * @brief   Main program body
  ******************************************************************************
  * @attention
  * tk_debug.c文件是调试时使用的文件，不建议修改。
  * 以下函数和变量用户不需要修改
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
  * @}
  */
#if TK_DEBUG_EN
#define USER_BAUD               (115200UL)
#define USER_UART_BAUD          ((SYSCLK-USER_BAUD)/(USER_BAUD))

/**
  * @brief  uart配置函数
  * @param  None
  * @retval None
  */
void debug_uart_config(void)
{
    UART1_BAUD1 = (USER_UART_BAUD >> 8) & 0xFF; // 配置波特率高八位
    UART1_BAUD0 =  USER_UART_BAUD & 0xFF;       // 配置波特率低八位
    UART1_CON0  =  UART_STOP_BIT(0x0) |         // 8bit数据，1bit停止位
                   UART_EN(0x1);
}

/**
  * @brief  重写putchar()函数
  * @param  None
  * @retval None
  */
void ll_uart_putchar(char uart_data)
{
    while(!(UART1_STA & UART_TX_DONE(0x01)));
    UART1_DATA = uart_data;
    while(!(UART1_STA & UART_TX_DONE(0x01)));
}

// 包头字节
#define TK_HEADER_BYTE              (0xA5)
#define PACK_SEND_COUNT_ONCE        (4)


#if (TK_CH_USE % PACK_SEND_COUNT_ONCE)
    #define PACK_IDX_MAX            (TK_CH_USE / PACK_SEND_COUNT_ONCE)
    #define PACK_SEND_COUNT_FINAL   (TK_CH_USE % PACK_SEND_COUNT_ONCE)
#else
    #define PACK_IDX_MAX            ((TK_CH_USE-1) / PACK_SEND_COUNT_ONCE)
    #define PACK_SEND_COUNT_FINAL   PACK_SEND_COUNT_ONCE
#endif

/**
  * @brief  调试打印函数.
  * @param  None
  * @retval None
  */
void tk_debug_func(void)
{
    static u8 xdata cur_index = 0;
    static u8 xdata pack_index = 0;
    static u16 xdata i_set_cnt = 0;
    u8 send_cnt = 0;
    
    TYPE_PACK_HEADER pack_header;
    TYPE_PACK_STRCUT pack_strcut;
    TYPE_PACK_TAIL   pack_tail;

    pack_header.pack_header = TK_HEADER_BYTE;
    pack_header.pack_curr_index = pack_index;
    
    if(pack_index == PACK_IDX_MAX)
    {
        send_cnt = PACK_SEND_COUNT_FINAL;
        pack_header.data_len = sizeof(pack_header) + sizeof(pack_strcut) * PACK_SEND_COUNT_FINAL + 2;
    } else {
        send_cnt = PACK_SEND_COUNT_ONCE;
        pack_header.data_len = sizeof(pack_header) + sizeof(pack_strcut) * PACK_SEND_COUNT_ONCE + 2;
    }
    
    tk_send_start(pack_header);
    
    do
    {
        pack_strcut.data1 = __tk_ch_data_0[cur_index];
        if (i_set_cnt++ < 1000) {
            pack_strcut.data2 = __tk_i_set[cur_index];
        } else {
            i_set_cnt = 1000;
            pack_strcut.data2 = __tk_ch_data_5[cur_index];
        }
        pack_strcut.data3 = __tk_ch_data_6[cur_index];
        tk_send_next(pack_strcut);
        send_cnt--;
        cur_index++;
    }while(send_cnt != 0);
    

    pack_tail.tk_flag = TK_THIS_TIME_VALID_FLAG(pack_index);

    tk_send_end(pack_tail);    
    
    if(pack_index == PACK_IDX_MAX)
    {
        pack_index = 0;
        cur_index = 0;
    } else {
        pack_index++;
    }
}
#endif

/*************************** (C) COPYRIGHT 2022 TAIXIN-IC ***** END OF FILE *****/
