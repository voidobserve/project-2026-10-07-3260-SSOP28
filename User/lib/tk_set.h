/**
  ******************************************************************************
  * @file     User/tk_set.h
  * @author   TAIXIN-IC Application Team
  * @version  V1.0.0
  * @date     05-20-2022
  * @brief    Main program body.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2022 TAIXIN-IC</center></h2>
  *
  *
  *
  ******************************************************************************
  */ 
  
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TK_SET_H
#define __TK_SET_H

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/

/* Exported constants --------------------------------------------------------*/
 
/** @defgroup XXX_LL_Register_Constants XXX LL Register Constants
  * @ingroup  XXX_LL_Driver
  * @brief    XXX LL register constant table definition
  *
  *
@verbatim   
  ===============================================================================
                                Register Constants
  ===============================================================================  
  
    Register Constants mainly encapsulates each bit in each group in the XXX 
    register. In the process of configuration, the macro definition can be directly 
    called to configure the XXX register, mainly for convenience. Understand the 
    configuration of the XXX.
    
@endverbatim
  *
  * @{
  */

#define TK_CH0_VALIB                                (1UL << 0)
#define TK_CH1_VALIB                                (1UL << 1)
#define TK_CH2_VALIB                                (1UL << 2)
#define TK_CH3_VALIB                                (1UL << 3)
#define TK_CH4_VALIB                                (1UL << 4)
#define TK_CH5_VALIB                                (1UL << 5)
#define TK_CH6_VALIB                                (1UL << 6)
#define TK_CH7_VALIB                                (1UL << 7)
#define TK_CH8_VALIB                                (1UL << 8)
#define TK_CH9_VALIB                                (1UL << 9)
#define TK_CH10_VALIB                               (1UL << 10)
#define TK_CH11_VALIB                               (1UL << 11)
#define TK_CH12_VALIB                               (1UL << 12)
#define TK_CH13_VALIB                               (1UL << 13)
#define TK_CH14_VALIB                               (1UL << 14)
#define TK_CH15_VALIB                               (1UL << 15)
#define TK_CH16_VALIB                               (1UL << 16)
#define TK_CH17_VALIB                               (1UL << 17)
#define TK_CH18_VALIB                               (1UL << 18)
#define TK_CH19_VALIB                               (1UL << 19)
#define TK_CH20_VALIB                               (1UL << 20)
#define TK_CH21_VALIB                               (1UL << 21)
#define TK_CH22_VALIB                               (1UL << 22)
#define TK_CH23_VALIB                               (1UL << 23)
#define TK_CH24_VALIB                               (1UL << 24)
#define TK_CH25_VALIB                               (1UL << 25)

// void tk_handle(void);

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


extern void tk_param_init(void);

#ifdef __cplusplus
}
#endif

#endif // __TK_SET__

/**
  * @}
  */

/**
  * @}
  */

/*************************** (C) COPYRIGHT 2021 TAIXIN-IC ***** END OF FILE *****/

