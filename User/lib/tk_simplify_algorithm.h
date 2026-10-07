/**
  ******************************************************************************
  * @file    tk_simplify_algorithm.h
  * @author  TAIXIN-IC Application Team
  * @version V1.0.0
  * @date    01-05-2021
  * @brief   This file contains all the GPIO LL firmware functions.
  ******************************************************************************
  * @attention
  *
  * 用户不允许修改
  *
  *
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __LIB_TK_ALGORITHM_H 
#define __LIB_TK_ALGORITHM_H
#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/


#include "tk_config.h"

extern volatile unsigned char    xdata __tk_update_cnt[TK_CH_USE];
extern volatile unsigned char    xdata __tk_confirm_cnt[TK_CH_USE]; 
extern volatile unsigned char    xdata __tk_leave_cnt[TK_CH_USE];
extern volatile unsigned char    xdata __tk_ch_index[TK_CH_USE];
extern volatile unsigned short   xdata __tk_ch_fth[TK_CH_USE];
extern volatile unsigned short   xdata __tk_i_set[TK_CH_USE];
extern volatile unsigned short   xdata __tk_ch_data_0[TK_CH_USE];
extern volatile unsigned short   xdata __tk_ch_data_1[TK_CH_USE];
extern volatile unsigned short   xdata __tk_ch_data_2[TK_CH_USE];
extern volatile unsigned short   xdata __tk_ch_data_3[TK_CH_USE];
extern volatile unsigned short   xdata __tk_ch_data_4[TK_CH_USE];
extern volatile unsigned short   xdata __tk_ch_data_5[TK_CH_USE];
extern volatile unsigned short   xdata __tk_ch_data_6[TK_CH_USE];

// 用户不允许修改
extern unsigned long  code  __tk_adjust_line;  
extern unsigned short code  __tk_adjust_time;
extern unsigned short code  __tk_adjust_diff_valu;
extern unsigned char  code  __tk_adjust_en;
extern unsigned short code  __tk_valid_time;
extern unsigned short code  __tk_long_key_time;
extern unsigned char  code  __tk_use_num;
extern unsigned char  code  __tk_cs_en;
extern unsigned char  code  __tk_nm_num;
extern unsigned char  code  __tk_tp_en;
extern unsigned char  code  __tk_base_update_cnt;
extern unsigned char  code  __tk_cm_valu;
extern unsigned short code  __tk_noise_value;
extern unsigned char  code  __tk_nm_cm_value;
// 用户不允许修改
extern void __tk_lib_init(void);
extern void __tk_scan(void);
extern void __tk_handler(void);
extern void __tk_ms_handler(void);
// 用户不允许修改
extern volatile bit __tk_long_key_flag;
extern volatile bit __tk_adjust_done_peng;
// 用户不允许修改
extern unsigned long data __tk_key_flag;
extern unsigned long data __tk_key_debug_flag;
extern unsigned char data __tk_ms_flag;
extern unsigned long data __tk_ch_en;
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif // __LIB_TK_ALGORITHM_H__

/**
  * @}
  */

/**
  * @}
  */

/*************************** (C) COPYRIGHT 2021 TAIXIN-IC ***** END OF FILE ****/
