
/**
  ******************************************************************************
  * @file     User/tk_debug.h
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
#ifndef __TK_DEBUG__
#define __TK_DEBUG__


#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
#define tk_send_byte(ch)        ll_uart_putchar(ch) 
/* Exported constants --------------------------------------------------------*/
void tk_debug_func(void);
void debug_uart_config(void);

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

/**
  * @}
  */
typedef struct _packheader
{
    u8  pack_header;            // 包头码
    u8  pack_curr_index;        // 包通道索引
    u8  data_len;               // 数据长度
} TYPE_PACK_HEADER;

typedef struct _packstrcut
{
    u16 data1;   
    u16 data2; 
    u16 data3; 
} TYPE_PACK_STRCUT;

typedef struct __packtail
{
    u8 tk_flag;
} TYPE_PACK_TAIL;


#define TK_THIS_TIME_VALID_FLAG(index)  ((__tk_key_debug_flag >> (index << 2)) & 0x0F)

// 最终发送函数                    
#define tk_send_start(pack_header)                                          \
{                                                                           \
    u8 __check_sum = 0;                                                     \
    u8 __i;                                                                 \
    u8 *__buf_point;                                                        \
    __buf_point = (u8*)&pack_header;                                        \
    for(__i = 0;__i < sizeof(pack_header);__i++,__buf_point++) {            \
        tk_send_byte(*__buf_point);                                         \
        __check_sum ^= *__buf_point;                                        \
    }
    
#define tk_send_next(pack_strcut)                                           \
    __buf_point = (u8*)&pack_strcut;                                        \
    for(__i = 0;__i < sizeof(pack_strcut);__i++,__buf_point++) {            \
        tk_send_byte(*__buf_point);                                         \
        __check_sum ^= *__buf_point;                                        \
    }

#define tk_send_end(pack_stop)                                              \
    __buf_point = (u8*)&pack_stop;                                          \
    for(__i = 0;__i < sizeof(pack_stop);__i++,__buf_point++) {              \
        tk_send_byte(*__buf_point);                                         \
        __check_sum ^= *__buf_point;                                        \
    }                                                                       \
    tk_send_byte(__check_sum);                                              \
}
/**
  * @}
  */

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif // __TK_DEBUG__

/**
  * @}
  */

/**
  * @}
  */

/*************************** (C) COPYRIGHT 2021 TAIXIN-IC ***** END OF FILE ****/
