/**
  ***********************************************************************************************
  * @file    User/tk_config.h
  * @author  HUGE-IC Application Team
  * @version V1.0.0
  * @date    05-20-2022
  * @brief   Main program body
  ***********************************************************************************************
  * @attention
  * 按键基础功能配置：CS开关配置、调试功能开关、长按键时间、按键消抖次数等；
  * 按键系统配置：按键通道使能、按键通道灵敏度配置；
  * 
  * 
  ***********************************************************************************************
  */ 
#ifndef __TK_CONFIG_H
#define __TK_CONFIG_H
/* Includes ------------------------------------------------------------------------------------*/
// #include "user_config.h"

/************************************************************************************************
******************************用户配置区：user configuration area********************************
*************************************************************************************************/
/* 按键基础功能配置 *****************************************************************************/

#define TK_CS_EN                        ( 0 )                 // 1: 开启CS功能;  0: 关闭CS功能
#define TK_TP_EN                        ( 0 )                 // 1: 开启TP功能;  0: 关闭TP功能
#define TK_DEBUG_EN                     ( 0 )                 // 1: 开启调试功能;  0: 关闭调试功能
#define TK_MU_CNT                       ( 1 )                 // 1：设1为防水模式，其他值组合键模式
#define TK_UPDATE_CNT                   ( 15 )                // BASE 更新时间(单位：10ms)
#define TK_LONG_KEY_TIME                ( 75 )                // 长按键时间(单位：10ms)
#define TK_VALID_TIME                   ( 500 )               // 按键有效时间(单位：10ms)
#define TK_NM_CM_CNT                    ( 4 )                 // 按键消抖次数
#define TK_NOISE_VAL                    ( 10 )                // 按键噪声值
#define TK_CM_VALE                      ( 5 )                 // 按键释放迟滞
#define TK_CH_MAX                       ( 26 )                // 配置最大按键个数
#define TK_CH_USE                       ( 2 )                 // 需要使用的按键个数


/* TK0 使能位
 * TK0 通道灵敏度
 */
#define TK0_CH_EN                       (0)
#define TK0_THR_DATA                    (100)
/*
 * TK1 使能位
 * TK1 通道灵敏度
 */
#define TK1_CH_EN                       (0)
#define TK1_THR_DATA                    (100)
/*
 * TK2 使能位
 * TK2 通道灵敏度
 */
#define TK2_CH_EN                       (0)
#define TK2_THR_DATA                    (100)
/*
 * TK3 使能位
 * TK3 通道灵敏度
 */
#define TK3_CH_EN                       (0)
#define TK3_THR_DATA                    (100)
/*
 * TK4 使能位
 * TK4 通道灵敏度
 */
#define TK4_CH_EN                       (0)
#define TK4_THR_DATA                    (100)
/*
 * TK5 使能位
 * TK5 通道灵敏度
 */
#define TK5_CH_EN                       (0)
#define TK5_THR_DATA                    (100)
/*
 * TK6 使能位
 * TK6 通道灵敏度
 */
#define TK6_CH_EN                       (0)
#define TK6_THR_DATA                    (100)
/*
 * TK7 使能位
 * TK7 通道灵敏度
 */
#define TK7_CH_EN                       (0)
#define TK7_THR_DATA                    (100)
/*
 * TK8 使能位
 * TK8 通道灵敏度
 */
#define TK8_CH_EN                       (0)
#define TK8_THR_DATA                    (100)
/*
 * TK9 使能位
 * TK9 通道灵敏度
 */
#define TK9_CH_EN                       (1)
// #define TK9_THR_DATA                    (1) // 
#define TK9_THR_DATA                    (10) // 
// #define TK9_THR_DATA                    (50) // 
// #define TK9_THR_DATA                    (100) // 
// #define TK9_THR_DATA                    (1500) // 这里调节到900，刚上电还是会一直触发长按
// #define TK9_THR_DATA                    (2000) // 还是会有概率触发长按
/*
 * TK10 使能位
 * TK10 通道灵敏度
 */
#define TK10_CH_EN                      (1)
// #define TK10_THR_DATA                   (1)
#define TK10_THR_DATA                   (10)
// #define TK10_THR_DATA                   (50)
// #define TK10_THR_DATA                   (100)
// #define TK10_THR_DATA                   (1500) 
// #define TK10_THR_DATA                   (2000) // 还是会有概率触发长按
/*
 * TK11 使能位
 * TK11 通道灵敏度
 */
#define TK11_CH_EN                      (0)
#define TK11_THR_DATA                   (100)
/*
 * TK12 使能位
 * TK12 通道灵敏度
 */
#define TK12_CH_EN                      (0)
#define TK12_THR_DATA                   (100)
/*
 * TK13 使能位
 * TK13 通道灵敏度
 */
#define TK13_CH_EN                      (0)
#define TK13_THR_DATA                   (100)
/*
 * TK14 使能位
 * TK14 通道灵敏度
 */
#define TK14_CH_EN                      (0)
#define TK14_THR_DATA                   (100)
/*
 * TK15 使能位
 * TK15 通道灵敏度
 */
#define TK15_CH_EN                      (0)
#define TK15_THR_DATA                   (100)
/*
 * TK16 使能位
 * TK16 通道灵敏度
 */
#define TK16_CH_EN                      (0)
#define TK16_THR_DATA                   (100)
/*
 * TK17 使能位
 * TK17 通道灵敏度
 */
#define TK17_CH_EN                      (0)
#define TK17_THR_DATA                   (100)
/*
 * TK18 使能位
 * TK18 通道灵敏度
 */
#define TK18_CH_EN                      (0)
#define TK18_THR_DATA                   (100)
/*
 * TK19 使能位
 * TK19 通道灵敏度
 */
#define TK19_CH_EN                      (0)
#define TK19_THR_DATA                   (100)
/*
 * TK20 使能位
 * TK20 通道灵敏度
 */
#define TK20_CH_EN                      (0)
#define TK20_THR_DATA                   (100)
/*
 * TK21 使能位
 * TK21 通道灵敏度
 */
#define TK21_CH_EN                      (0)
#define TK21_THR_DATA                   (100)
/*
 * TK22 使能位
 * TK22 通道灵敏度
 */
#define TK22_CH_EN                      (0)
#define TK22_THR_DATA                   (100)
/*
 * TK23 使能位
 * TK23 通道灵敏度
 */
#define TK23_CH_EN                      (0)
#define TK23_THR_DATA                   (100)
/*
 * TK24 使能位
 * TK24 通道灵敏度
 */
#define TK24_CH_EN                      (0)
#define TK24_THR_DATA                   (100)
/*
 * TK25 使能位
 * TK25 通道灵敏度
 */
#define TK25_CH_EN                      (0)
#define TK25_THR_DATA                   (100)


/**************************************************************************************************
*************************系统配置区：System configuration area*************************************
***************************************************************************************************/
#define TK_RES_GEAR                     (6)                 // 放电电阻
#define TK_CURR_GEAR                    (65)                // 充电电流
#define TK_DATA_LINE                    (2500)              // 按键目标线
#define TK_ADJUST_TIME                  (100)               // 按键初始化时间(单位：10ms)
#define TK_MAX_DIFF_VALU                (200)               // 初始化最大差值
#define TK_ADJUST_EN                    (1)                 // 是否进行电流源自适应

#endif // end  #if __TK_CONFIG_H
/******************************************************************************************/
