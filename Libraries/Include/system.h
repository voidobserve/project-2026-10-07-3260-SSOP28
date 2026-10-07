/**
  ******************************************************************************
  * @file    system.h
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
#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#ifdef __cplusplus
 extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "typedef.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

#define NVR_START_RADDR             (0x4000)                                                // NVR区的起始地址

#define ADC_IBTRIM_VAL              ((*((u8 code *)(NVR_START_RADDR + 16)) >> 0) & 0x01)    // ADC失调电流校准
#define ADC_TRIM_VAL                ((*((u8 code *)(NVR_START_RADDR + 16)) >> 1) & 0x3F)    // ADC校准值
#define BGR_TRIM                    ((*((u8 code *)(NVR_START_RADDR + 17)) >> 0) & 0x07)    // PMU基准校准值
#define CMP0_TRIM_VAL               ((*((u8 code *)(NVR_START_RADDR + 18)) >> 0) & 0x0F)    // CMP0校准值
#define CMP1_TRIM_VAL               ((*((u8 code *)(NVR_START_RADDR + 18)) >> 4) & 0x0F)    // CMP1校准值
#define CMP_TRIMIB_VAL              ((*((u8 code *)(NVR_START_RADDR + 19)) >> 0) & 0x3F)    // 20mA电流源校准值
#define RC64K_TRIM_VAL              ((*((u8 code *)(NVR_START_RADDR + 23)) >> 0) & 0x7F)    // 内部RC64K校准值
#define AMP0_TRIM_VAL               (*((u8 code *)(NVR_START_RADDR + 20)))                  // AMP0校准值
#define AMP1_TRIM_VAL               (*((u8 code *)(NVR_START_RADDR + 21)))                  // AMP1校准值
#define AMP2_TRIM_VAL               (*((u8 code *)(NVR_START_RADDR + 22)))                  // AMP1校准值
// 装载芯片各个模块校准值
#define __LOADER_CHIP_TRIM                                                                   \
do {                                                                                         \
        PMU_CON3  = ((PMU_CON3  & 0xF8) | BGR_TRIM);                    /* PMU基准校准    */ \
        PMU_CON4  = ((PMU_CON4  & 0x80) | RC64K_TRIM_VAL);              /* 内部RC64K校准  */ \
        ADC_ACON1 = ((ADC_ACON1 & 0x7F) | (ADC_IBTRIM_VAL << 7));       /* ADC失调电流    */ \
        ADC_ACON2 = ((ADC_ACON2 & 0x80) | ((0x1 << 6) | ADC_TRIM_VAL)); /* ADC校准        */ \
        CMP0_CON4 = ((CMP0_CON4 & 0xF0) | CMP0_TRIM_VAL);               /* CMP0校准       */ \
        CMP1_CON4 = ((CMP1_CON4 & 0xF0) | CMP1_TRIM_VAL);               /* CMP1校准       */ \
        CMP_CON   = ((CMP_CON   & 0xC0) | CMP_TRIMIB_VAL);              /* 20mA电流源校准 */ \
        AMP_CON2  = (AMP0_TRIM_VAL);                                    /* AMP0校准       */ \
        AMP_CON4  = (AMP1_TRIM_VAL);                                    /* AMP1校准       */ \
        AMP_CON6  = (AMP2_TRIM_VAL);                                    /* AMP1校准       */ \
} while(0)

#define __HIRC_DELAY                do{delay(5);delay_ms(0);}while(0)                   // HIRC稳定延时
     
// 系统时钟    注意:改变这个宏不能改变系统时钟，需要调用 __SET_SYSCLK
#define SYSCLK                      (48000000UL)
// 系统时钟选择项
#define SYSCLK_LIRC64K              (0x0)
#define SYSCLK_XOSCM                (0x1)
#define SYSCLK_HIRC48M_DIV0         (0x2)
#define SYSCLK_HIRC48M_DIV2         (0x3)
#define SYSCLK_HIRC48M_DIV3         (0x4)
#define SYSCLK_HIRC48M_DIV4         (0x5)
#define SYSCLK_HIRC48M_DIV5         (0x6)
#define SYSCLK_HIRC48M_DIV6         (0x7)
#define SYSCLK_HIRC48M_DIV7         (0x8)
#define SYSCLK_HIRC48M_DIV8         (0x9)
// 系统时钟分频项
#define SYSCLK_DIV0                 (0x1)
#define SYSCLK_DIV2                 (0x2)
#define SYSCLK_DIV3                 (0x3)
#define SYSCLK_DIV4                 (0x4)
#define SYSCLK_DIV5                 (0x5)
#define SYSCLK_DIV6                 (0x6)
#define SYSCLK_DIV7                 (0x7)
#define SYSCLK_DIV8                 (0x8)
#define SYSCLK_DIV9                 (0x9)
#define SYSCLK_DIV10                (0xA)
#define SYSCLK_DIV11                (0xB)
#define SYSCLK_DIV12                (0xC)
#define SYSCLK_DIV13                (0xD)
#define SYSCLK_DIV14                (0xE)
#define SYSCLK_DIV15                (0xF)
// 系统时钟设置
#define __SET_SYSCLK(clk_sel, clk_div)                                                                                            \
do {                                                                                                                              \
    if((clk_sel == SYSCLK_LIRC64K) || (clk_sel == SYSCLK_XOSCM)) {                                                                \
        CLK_ACON0 &= ~CLK_AIP_HRC_EN(0x1);                  /* 关闭 HIRC                        */                                \
    } else {                                                                                                                      \
        CLK_ACON0 |= CLK_AIP_HRC_EN(0x1);                   /* 使能 HIRC                        */                                \
        __HIRC_DELAY;                                       /* 等待 HIRC 稳定                   */                                \
    }                                                                                                                             \
    FLASH_TIMEREG0 = 0x55;                                                                                                        \
                                                                                                                                  \
    if(clk_sel == SYSCLK_LIRC64K) {                                                                                               \
        FLASH_TIMEREG1 = 0x50;                              /* FLASH访问速度 = 系统时钟         */                                \
                                                            /* 系统时钟分频                     */                                \
        CLK_CON2 = (CLK_CON2  & (~CLK_SYS_DIV(0xF))) | CLK_SYS_DIV(clk_div-1);                                                    \
                                                            /* 系统时钟选择 RC64                */                                \
        CLK_CON0 = (CLK_CON0 & (~CLK_SYSCLK_SEL(0x3))) | CLK_SYSCLK_SEL(0x0);                                                     \
                                                            /* FLASH烧写时钟 = 系统时钟         */                                \
        CLK_CON6 = (CLK_CON6 & (~CLK_FLASH_DIV(0x3F))) | CLK_FLASH_DIV(0x00);                                                     \
                                                                                                                                  \
    } else if(clk_sel == SYSCLK_XOSCM) {                                                                                          \
        FLASH_TIMEREG1 = 0x50;                              /* FLASH访问速度 = 系统时钟         */                                \
                                                            /* 系统时钟分频                     */                                \
        CLK_CON2 = (CLK_CON2  & (~CLK_SYS_DIV(0xF))) | CLK_SYS_DIV(clk_div-1);                                                    \
                                                            /* 系统时钟选择 XOSCM               */                                \
        CLK_CON0 = (CLK_CON0 & (~CLK_SYSCLK_SEL(0x3))) | CLK_SYSCLK_SEL(0x1);                                                     \
                                                            /* FLASH烧写时钟 = 系统时钟         */                                \
        CLK_CON6 = (CLK_CON6 & (~CLK_FLASH_DIV(0x3F))) | CLK_FLASH_DIV(0x00);                                                     \
                                                                                                                                  \
    } else if(clk_sel == SYSCLK_HIRC48M_DIV0) {                                                                                   \
        if(clk_div < SYSCLK_DIV4) {                                                                                               \
            FLASH_TIMEREG1 = 0x54;                          /* FLASH访问速度 = 跟随系统时钟/2   */                                \
        } else {                                                                                                                  \
            FLASH_TIMEREG1 = 0x50;                          /* FLASH访问速度 = 系统时钟         */                                \
        }                                                                                                                         \
                                                            /* 系统时钟分频                     */                                \
        CLK_CON2 = (CLK_CON2  & (~CLK_SYS_DIV(0xF))) | CLK_SYS_DIV(clk_div-1);                                                    \
                                                            /* 系统时钟选择 HIRC48M             */                                \
        CLK_CON0 = (CLK_CON0 & (~CLK_SYSCLK_SEL(0x3))) | CLK_SYSCLK_SEL(0x3);                                                     \
                                                            /* FLASH烧写时钟48分频：1M          */                                \
        CLK_CON6 = (CLK_CON6 & (~CLK_FLASH_DIV(0x3F))) | CLK_FLASH_DIV(48/clk_div - 1);                                           \
                                                                                                                                  \
    } else {                                                                                                                      \
        if(((clk_sel + 1 - SYSCLK_HIRC48M_DIV2)*clk_div) < 4) {                                                                   \
            FLASH_TIMEREG1 = 0x54;                          /* FLASH访问速度 = 跟随系统时钟/2   */                                \
        } else {                                                                                                                  \
            FLASH_TIMEREG1 = 0x54;                          /* FLASH访问速度 = 系统时钟         */                                \
        }                                                                                                                         \
                                                                                                                                  \
                                                                                                                                  \
        CLK_CON1 = (CLK_CON1 & (~CLK_HIRC_DIV(0x7)))   | CLK_HIRC_DIV(clk_sel + 1 - SYSCLK_HIRC48M_DIV2);                         \
                                                            /* 系统时钟分频                     */                                \
        CLK_CON2 = (CLK_CON2  & (~CLK_SYS_DIV(0xF))) | CLK_SYS_DIV(clk_div-1);                                                    \
                                                            /* 系统时钟选择 HIRC48M分频        */                                 \
        CLK_CON0 = (CLK_CON0 & (~CLK_SYSCLK_SEL(0x3))) | CLK_SYSCLK_SEL(0x2);                                                     \
        if(((clk_sel + 1 - SYSCLK_HIRC48M_DIV2)*clk_div) > 48) {                                                                  \
                                                            /* FLASH烧写时钟 = 系统时钟         */                                \
            CLK_CON6 = (CLK_CON6 & (~CLK_FLASH_DIV(0x3F))) | CLK_FLASH_DIV(0x00);                                                 \
        } else {                                                                                                                  \
                                                            /* FLASH烧写时钟48分频: 1M          */                                \
            CLK_CON6 = (CLK_CON6 & (~CLK_FLASH_DIV(0x3F))) | CLK_FLASH_DIV((48/(((clk_sel+1-SYSCLK_HIRC48M_DIV2)*clk_div)+1)-1)); \
        }                                                                                                                         \
    }                                                                                                                             \
} while(0)

/****************** Peripheral Interrupt Numbers ****************************/
// 中断号   
#define LVD_IRQn              0        // IE0    IP0
#define P0_IRQn               1
#define P1_IRQn               2
#define P2_IRQn               3
#define P3_IRQn               4        //        IP1
#define TMR0_IRQn             5
#define TMR1_IRQn             6
#define TMR2_IRQn             7        // IE1    IP2
#define TMR3_IRQn             8
#define TMR4_IRQn             9
#define STMR0_IRQn           10
#define STMR1_IRQn           11        //        IP3
#define STMR2_IRQn           12
#define STMR3_IRQn           13
#define STMR4_IRQn           14
#define STMR5_IRQn           15        // IE2    IP4
#define WUT_IRQn             16
#define ADC_IRQn             17
#define COMP_IRQn            18
#define IIC_IRQn             19        //        IP5
#define UART0_IRQn           20
#define UART1_IRQn           21
#define SPI0_IRQn            22
#define WKPND_IRQn           23        // IE3    IP6
#define WDT_IRQn             24
#define BUZ_IRQn             25
#define TK_IRQn              26
#define AMP_IRQn             27
#define LED_IRQn             28

// 中断优先级配置函数，标准SDK只支持2级优先级，需要2级以上的请咨询FAE
#define LVD_IQn_CFG          (0)       // 0：优先级为0      1：优先级为1
#define P0_IQn_CFG           (0)       // 0：优先级为0      1：优先级为1
#define P1_IQn_CFG           (0)       // 0：优先级为0      1：优先级为1
#define P2_IQn_CFG           (0)       // 0：优先级为0      1：优先级为1
#define P3_IQn_CFG           (0)       // 0：优先级为0      1：优先级为1
#define TMR0_IQn_CFG         (0)       // 0：优先级为0      1：优先级为1
#define TMR1_IQn_CFG         (0)       // 0：优先级为0      1：优先级为1
#define TMR2_IQn_CFG         (0)       // 0：优先级为0      1：优先级为1
#define TMR3_IQn_CFG         (0)       // 0：优先级为0      1：优先级为1
#define TMR4_IQn_CFG         (0)       // 0：优先级为0      1：优先级为1
#define STMR0_IQn_CFG        (0)       // 0：优先级为0      1：优先级为1
#define STMR1_IQn_CFG        (0)       // 0：优先级为0      1：优先级为1
#define STMR2_IQn_CFG        (0)       // 0：优先级为0      1：优先级为1
#define STMR3_IQn_CFG        (0)       // 0：优先级为0      1：优先级为1
#define STMR4_IQn_CFG        (0)       // 0：优先级为0      1：优先级为1
#define STMR5_IQn_CFG        (0)       // 0：优先级为0      1：优先级为1

#define WUT_IQn_CFG          (0)       // 0：优先级为0      1：优先级为1
#define ADC_IQn_CFG          (0)       // 0：优先级为0      1：优先级为1
#define COMP_IQn_CFG         (0)       // 0：优先级为0      1：优先级为1
#define IIC_IQn_CFG          (0)       // 0：优先级为0      1：优先级为1
#define UART0_IQn_CFG        (0)       // 0：优先级为0      1：优先级为1
#define UART1_IQn_CFG        (0)       // 0：优先级为0      1：优先级为1
#define SPI0_IQn_CFG         (0)       // 0：优先级为0      1：优先级为1
#define WKPND_IQn_CFG        (0)       // 0：优先级为0      1：优先级为1
#define WDT_IQn_CFG          (0)       // 0：优先级为0      1：优先级为1
#define BUZ_IQn_CFG          (0)       // 0：优先级为0      1：优先级为1
#define TK_IQn_CFG           (0)       // 0：优先级为0      1：优先级为1
#define AMP_IQn_CFG          (0)       // 0：优先级为0      1：优先级为1
#define LED_IQn_CFG          (0)       // 0：优先级为0      1：优先级为1

#define IP0_REG_CFG          ((LVD_IQn_CFG   << 0) | (P0_IQn_CFG    << 2) | (P1_IQn_CFG    << 4) | (P2_IQn_CFG    << 6))
#define IP1_REG_CFG          ((P3_IQn_CFG    << 0) | (TMR0_IQn_CFG  << 2) | (TMR1_IQn_CFG  << 4))
#define IP2_REG_CFG          ((TMR2_IQn_CFG  << 0) | (TMR3_IQn_CFG  << 2) | (TMR4_IQn_CFG  << 4) | (STMR0_IQn_CFG << 6))
#define IP3_REG_CFG          ((STMR1_IQn_CFG << 0) | (STMR2_IQn_CFG << 2) | (STMR3_IQn_CFG << 4) | (STMR4_IQn_CFG << 6))
#define IP4_REG_CFG          ((STMR5_IQn_CFG << 0) | (WUT_IQn_CFG   << 2) | (ADC_IQn_CFG   << 4) | (COMP_IQn_CFG  << 6))
#define IP5_REG_CFG          ((IIC_IQn_CFG   << 0) | (UART0_IQn_CFG << 2) | (UART1_IQn_CFG << 4) | (SPI0_IQn_CFG  << 6))
#define IP6_REG_CFG          ((WKPND_IQn_CFG << 0) | (WDT_IQn_CFG   << 2) | (BUZ_IQn_CFG   << 4) | (TK_IQn_CFG    << 6))
#define IP7_REG_CFG          ((AMP_IQn_CFG   << 0) | (LED_IQn_CFG   << 2))

#define IP_REG_CFG_16L       ((LVD_IQn_CFG   << 0 ) | (P0_IQn_CFG    << 1 ) | (P1_IQn_CFG    << 2)  | (P2_IQn_CFG    << 3 ) |  \
                              (P3_IQn_CFG    << 4 ) | (TMR0_IQn_CFG  << 5 ) | (TMR1_IQn_CFG  << 6)  | (TMR2_IQn_CFG  << 7 ) |  \
                              (TMR3_IQn_CFG  << 8 ) | (TMR4_IQn_CFG  << 9 ) | (STMR0_IQn_CFG << 10) | (STMR1_IQn_CFG << 11) |  \
                              (STMR2_IQn_CFG << 12) | (STMR3_IQn_CFG << 13) | (STMR4_IQn_CFG << 14) | (STMR5_IQn_CFG << 15))
#define IP_REG_CFG_16H       ((WUT_IQn_CFG   << 0 ) | (ADC_IQn_CFG   << 1 ) | (COMP_IQn_CFG  << 2)  | (IIC_IQn_CFG   << 3 ) |  \
                              (UART0_IQn_CFG << 4 ) | (UART1_IQn_CFG << 5 ) | (SPI0_IQn_CFG  << 6)  | (WKPND_IQn_CFG << 7 ) |  \
                              (WDT_IQn_CFG   << 8 ) | (BUZ_IQn_CFG   << 9 ) | (TK_IQn_CFG    << 10) | (AMP_IQn_CFG   << 11) |  \
                              (LED_IQn_CFG   << 12))

// 使能中断
#define __EnableIRQ(IRQn)                                             \
do {                                                                  \
    if(IRQn < 7)                         { IE0 |= (1 << IRQn);      } \
    else if((IRQn >= 7)  && (IRQn < 15)) { IE1 |= (1 << (IRQn-7));  } \
    else if((IRQn >= 15) && (IRQn < 23)) { IE2 |= (1 << (IRQn-15)); } \
    else if((IRQn >= 23) && (IRQn < 31)) { IE3 |= (1 << (IRQn-23)); } \
} while(0);

// 关闭中断
#define __DisableIRQ(IRQn)                                         \
do {                                                               \
    if(IRQn < 7)                       { IE0 &= ~(1<<IRQn);      } \
    else if((IRQn >= 7) &&(IRQn < 15)) { IE1 &= ~(1<<(IRQn-7));  } \
    else if((IRQn >= 15)&&(IRQn < 23)) { IE2 &= ~(1<<(IRQn-15)); } \
    else if((IRQn >= 23)&&(IRQn < 31)) { IE2 &= ~(1<<(IRQn-23)); } \
} while(0);

// 设置中断的优先级
#define __SetIRQnIP(IRQn, IPn)                                                                                    \
do {                                                                                                              \
    if(IRQn < 4)                         { IP0 |= (IP0 & (~(0x3 << (IRQn*2))))      | ((IPn) << (IRQn*2));      } \
    else if((IRQn >= 4)  && (IRQn < 7))  { IP1 |= (IP1 & (~(0x3 << ((IRQn-4)*2))))  | ((IPn) << ((IRQn-4)*2));  } \
    else if((IRQn >= 7)  && (IRQn < 11)) { IP2 |= (IP2 & (~(0x3 << ((IRQn-7)*2))))  | ((IPn) << ((IRQn-7)*2));  } \
    else if((IRQn >= 11) && (IRQn < 15)) { IP3 |= (IP3 & (~(0x3 << ((IRQn-11)*2)))) | ((IPn) << ((IRQn-11)*2)); } \
    else if((IRQn >= 15) && (IRQn < 19)) { IP4 |= (IP4 & (~(0x3 << ((IRQn-15)*2)))) | ((IPn) << ((IRQn-15)*2)); } \
    else if((IRQn >= 19) && (IRQn < 23)) { IP5 |= (IP5 & (~(0x3 << ((IRQn-19)*2)))) | ((IPn) << ((IRQn-19)*2)); } \
    else if((IRQn >= 23) && (IRQn < 27)) { IP6 |= (IP6 & (~(0x3 << ((IRQn-23)*2)))) | ((IPn) << ((IRQn-23)*2)); } \
    else if((IRQn >= 27) && (IRQn < 29)) { IP7 |= (IP7 & (~(0x3 << ((IRQn-27)*2)))) | ((IPn) << ((IRQn-27)*2)); } \
} while(0);

// 进入中断设置IP
#define __IRQnIPnPush(IRQn)                                          \
do {                                                                 \
    if(!((1UL<<IRQn) & ((IP_REG_CFG_16L)|(IP_REG_CFG_16H << 16)))) { \
        if(IP0_REG_CFG) { IP0 = IP0_REG_CFG << 1; }                  \
        if(IP1_REG_CFG) { IP1 = IP1_REG_CFG << 1; }                  \
        if(IP2_REG_CFG) { IP2 = IP2_REG_CFG << 1; }                  \
        if(IP3_REG_CFG) { IP3 = IP3_REG_CFG << 1; }                  \
        if(IP4_REG_CFG) { IP4 = IP4_REG_CFG << 1; }                  \
        if(IP5_REG_CFG) { IP5 = IP5_REG_CFG << 1; }                  \
        if(IP6_REG_CFG) { IP6 = IP6_REG_CFG << 1; }                  \
        if(IP7_REG_CFG) { IP7 = IP7_REG_CFG << 1; }                  \
    }                                                                \
} while(0);

// 退出中断设置IP
#define __IRQnIPnPop(IRQn)                                           \
do {                                                                 \
    if(!((1UL<<IRQn) & ((IP_REG_CFG_16L)|(IP_REG_CFG_16H << 16)))) { \
        if(IP0_REG_CFG) { IP0 = IP0_REG_CFG; }                       \
        if(IP1_REG_CFG) { IP1 = IP1_REG_CFG; }                       \
        if(IP2_REG_CFG) { IP2 = IP2_REG_CFG; }                       \
        if(IP3_REG_CFG) { IP3 = IP3_REG_CFG; }                       \
        if(IP4_REG_CFG) { IP4 = IP4_REG_CFG; }                       \
        if(IP5_REG_CFG) { IP5 = IP5_REG_CFG; }                       \
        if(IP6_REG_CFG) { IP6 = IP6_REG_CFG; }                       \
        if(IP7_REG_CFG) { IP7 = IP7_REG_CFG; }                       \
    }                                                                \
} while(0);



/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/** @addtogroup Template_Project
  * @{
  */

/** @addtogroup SYSTEM
  * @{
  */ 

     
/** @defgroup SYSTEM_Exported_Constants
  * @{
  */
/**
  * @}
  */


/** @defgroup SYSTEM_Exported_Functions
  * @{
  */

  /**
  * @brief  Delay function
  * @param  cnt : the number of cnt to delay
  * @retval None
  */
void delay(u16 cnt);

/**
  * @brief  Delay function
  * @param  ms : the number of ms to delay
  * @retval None
  */
void delay_ms(u16 ms);

/**
  * @brief  System initialization function
  * @param  None
  * @retval None
  */
void system_init(void);


/**
  * @}
  */ 
     

#ifdef __cplusplus
}
#endif

#endif //__SYSTEM_H__

/**
  * @}
  */

/**
  * @}
  */

/*************************** (C) COPYRIGHT 2021 HUGE-IC ***** END OF FILE *****/
