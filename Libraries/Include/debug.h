/**
  ******************************************************************************
  * @file    debug.h
  * @author  HUGE-IC Application Team
  * @version V1.0.0
  * @date    01-05-2021
  * @brief   Main program body header file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2021 HUGE-IC</center></h2>
  *
  *
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __DEBUG_H__
#define __DEBUG_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

// 使用%d打印的数值不对时，请注意：
// 格式------含义----------针对类型
// % d--- 两个字节变量 ---- int
// %bd--- 单个字节变量 ---- char
// %ld--- 四个字节变量 ---- long int

#define DBG_PRINTF_EN           (0)             // 0:禁用标准printf()函数, 1:使能标准printf()函数

#define DEBUG_BAUD              (115200UL)
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
     
/** @addtogroup Template_Project
  * @{
  */

/** @addtogroup DEBUG
  * @{
  */ 

     
/** @defgroup DEBUG_Exported_Constants
  * @{
  */
/**
  * @}
  */


/** @defgroup DEBUG_Exported_Functions
  * @{
  */
  
#if 0
/**
  * @brief  Debug puchar function
  * @param  uart_data : Data that needs to be sent
  * @retval None
  */
void debug_putchar(u8 uart_data);

/**
  * @brief  Debug initialization function
  * @param  None
  * @retval None
  */
void debug_init(void);

/**
  * @brief  Debug User printf function
  * @param  p_data : The string to send
  * @retval None
  */
void user_printf(char *p_data);
#endif

/**
  * @}
  */ 


#ifdef __cplusplus
}
#endif

#endif //__DEBUG_H__

/**
  * @}
  */

/**
  * @}
  */

/*************************** (C) COPYRIGHT 2021 HUGE-IC ***** END OF FILE *****/
