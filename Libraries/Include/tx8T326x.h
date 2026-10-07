#ifndef __TX8T326x_H__
#define __TX8T326x_H__

//==========================================================================//
//用xsfr页切换配置流程:                                                     //
//step1:xsfr>=256时，需要操作寄存器，需要先配PCON1 = 0x81；                 //
//step2:操作完之后，需要配PCON1 = 0x80;                                     //
//==========================================================================//

#define CHIP_BASE           0x04
#define LED_BASE            0x07
#define STMR_BASE           0x0e
#define ADC_BASE            0x69
#define TK_BASE             0x70
#define TMR0_BASE           0x90
#define TMR1_BASE           0x9C
#define IODRV_BASE          0xA8
#define P0_BASE             0xC0
#define P1_BASE             0xD0
#define P2_BASE             0xE0
#define P3_BASE             0xF0
#define TMR2_BASE           0x7100
#define TMR3_BASE           0x7108
#define SYS_BASE            0x7110
#define PMUCON_BASE         0x7119
#define AIPCON_BASE         0x7120
#define CLKCON_BASE         0x7130
#define CMP_BASE            0x7140
#define WUT_BASE            0x7152
#define LVD_BASE            0x715A
#define SYSCTRL0_BASE       0x715E
#define BUZ_BASE            0x7164
#define IOFUNC_BASE         0x7166


// IP CORE
sfr     SP                  = 0x81;
sfr     DPL0                = 0x82;
sfr     DPH0                = 0x83;
sfr     DPL1                = 0x84;
sfr     DPH1                = 0x85;
sfr     DPCFG               = 0x86;
sfr     SPH                 = 0x9B;
sfr     PCON1               = 0x9C;
sfr     PSW                 = 0xD0;
sbit    PSW_CY              = 0xD7;
sbit    PSW_AC              = 0xD6;
sbit    PSW_F0              = 0xD5;
sbit    PSW_RS1             = 0xD4;
sbit    PSW_RS0             = 0xD3;
sbit    PSW_OV              = 0xD2;
sbit    PSW_F1              = 0xD1;
sbit    PSW_PARITY          = 0xD0;
sfr     ACC                 = 0xE0;
sbit    ACC_7               = 0xE7;
sbit    ACC_6               = 0xE6;
sbit    ACC_5               = 0xE5;
sbit    ACC_4               = 0xE4;
sbit    ACC_3               = 0xE3;
sbit    ACC_2               = 0xE2;
sbit    ACC_1               = 0xE1;
sbit    ACC_0               = 0xE0;
sfr     B                   = 0xF0;

// CLK  PCON1 = 0x81;
#define CLK_ACON0           *(unsigned char volatile xdata *)(AIPCON_BASE+0x0)
#define CLK_ACON1           *(unsigned char volatile xdata *)(AIPCON_BASE+0x1)
#define CLK_XOSC            *(unsigned char volatile xdata *)(SYSCTRL0_BASE+0x4)
#define CLK_CON0            *(unsigned char volatile xdata *)(CLKCON_BASE+0x0)
#define CLK_CON1            *(unsigned char volatile xdata *)(CLKCON_BASE+0x1)
#define CLK_CON2            *(unsigned char volatile xdata *)(CLKCON_BASE+0x2)
#define CLK_CON3            *(unsigned char volatile xdata *)(CLKCON_BASE+0x3)
#define CLK_CON4            *(unsigned char volatile xdata *)(CLKCON_BASE+0x4)
#define CLK_CON5            *(unsigned char volatile xdata *)(CLKCON_BASE+0x5)
#define CLK_CON6            *(unsigned char volatile xdata *)(CLKCON_BASE+0x6)
#define CLK_CON7            *(unsigned char volatile xdata *)(CLKCON_BASE+0x7)
#define CLK_CON8            *(unsigned char volatile xdata *)(CLKCON_BASE+0x8)

// WDT
sfr     WDT_CON             = 0xD8;
sfr     WDT_KEY             = 0xD9;

// LVD  PCON1 = 0x81;
#define LVD_CON0            *(unsigned char volatile xdata *)(LVD_BASE+0x0)
#define LVD_CON1            *(unsigned char volatile xdata *)(LVD_BASE+0x1)
#define LVD_CON2            *(unsigned char volatile xdata *)(LVD_BASE+0x2)
#define LVD_CON3            *(unsigned char volatile xdata *)(LVD_BASE+0x3)

// LP
sfr     LP_CON              = 0xBA;
sfr     LP_WKCON            = 0x8D;
sfr     LP_WKPND            = 0x8E;

// SYS  PCON1 = 0x81;
#define SYS_CON0            *(unsigned char volatile xdata *)(SYS_BASE+0x0)
#define SYS_CON1            *(unsigned char volatile xdata *)(SYS_BASE+0x1)
#define SYS_CON2            *(unsigned char volatile xdata *)(SYS_BASE+0x2)
#define SYS_CON3            *(unsigned char volatile xdata *)(SYS_BASE+0x3)
#define SYS_CON4            *(unsigned char volatile xdata *)(SYS_BASE+0x4)
#define SYS_CON5            *(unsigned char volatile xdata *)(SYS_BASE+0x5)
#define SYS_CON6            *(unsigned char volatile xdata *)(SYS_BASE+0x6)
#define SYS_CON7            *(unsigned char volatile xdata *)(SYS_BASE+0x7)
#define SYS_CON8            *(unsigned char volatile xdata *)(SYS_BASE+0x8)
#define IO_MAP              *(unsigned char volatile xdata *)(SYSCTRL0_BASE+0x0)
sfr     SYS_PND             = 0x8F;

// PMU  PCON1 = 0x81;
#define PMU_CON0            *(unsigned char volatile xdata *)(PMUCON_BASE+0x0)
#define PMU_CON1            *(unsigned char volatile xdata *)(PMUCON_BASE+0x1)
#define PMU_CON2            *(unsigned char volatile xdata *)(PMUCON_BASE+0x2)
#define PMU_CON3            *(unsigned char volatile xdata *)(PMUCON_BASE+0x3)
#define PMU_CON4            *(unsigned char volatile xdata *)(PMUCON_BASE+0x4)
#define PMU_CON5            *(unsigned char volatile xdata *)(PMUCON_BASE+0x5)
#define PMU_CON6            *(unsigned char volatile xdata *)(PMUCON_BASE+0x6)
#define PMU_BK              *(unsigned char volatile xdata *)(SYSCTRL0_BASE+0x2)

// IRQ
sfr     IE0                 = 0xA8;
sfr     IE1                 = 0xA9;
sfr     IE2                 = 0xAA;
sfr     IE3                 = 0xAB;
sbit    IE_EA               = 0xAF;
sfr     IP0                 = 0xB8;
sfr     IP1                 = 0xB1;
sfr     IP2                 = 0xB2;
sfr     IP3                 = 0xB3;
sfr     IP4                 = 0xB4;
sfr     IP5                 = 0xB5;
sfr     IP6                 = 0xB6;
sfr     IP7                 = 0xB7;

// GPIO P0
sfr     P0                  = 0x80;
sbit    P00                 = 0x80;
sbit    P01                 = 0x81;
sbit    P02                 = 0x82;
sbit    P03                 = 0x83;
sbit    P04                 = 0x84;
sbit    P05                 = 0x85;
sbit    P06                 = 0x86;
sbit    P07                 = 0x87;
// GPIO P0 PCON1 = 0x80;
#define P0_PU               *(unsigned char volatile pdata *)(P0_BASE+0x0)
#define P0_PD               *(unsigned char volatile pdata *)(P0_BASE+0x1)
#define P0_MD0              *(unsigned char volatile pdata *)(P0_BASE+0x2)
#define P0_MD1              *(unsigned char volatile pdata *)(P0_BASE+0x3)
#define P0_TRG0             *(unsigned char volatile pdata *)(P0_BASE+0x4)
#define P0_TRG1             *(unsigned char volatile pdata *)(P0_BASE+0x5)
#define P0_PND              *(unsigned char volatile pdata *)(P0_BASE+0x6)
#define P0_IMK              *(unsigned char volatile pdata *)(P0_BASE+0x7)
#define P0_AIOEN            *(unsigned char volatile pdata *)(P0_BASE+0x8)
#define P0_AIOEN1           *(unsigned char volatile pdata *)(P0_BASE+0xC)
#define P0_AIOEN2           *(unsigned char volatile pdata *)(P0_BASE+0xD)
#define P0_DRV0             *(unsigned char volatile pdata *)(0xC0+0x9)
#define P0_DRV1             *(unsigned char volatile pdata *)(IODRV_BASE+0x0)
#define P0_DRV2             *(unsigned char volatile pdata *)(IODRV_BASE+0x1)
#define P0_DRV3             *(unsigned char volatile pdata *)(IODRV_BASE+0x2)
#define P0_DRV4             *(unsigned char volatile pdata *)(IODRV_BASE+0x3)
#define P0_DRV5             *(unsigned char volatile pdata *)(IODRV_BASE+0x4)
#define P0_DRV6             *(unsigned char volatile pdata *)(IODRV_BASE+0x5)
#define P0_DRV7             *(unsigned char volatile pdata *)(IODRV_BASE+0x6)
#define P0_ODN              *(unsigned char volatile pdata *)(P0_BASE+0xA)
#define P0_ODP              *(unsigned char volatile pdata *)(P0_BASE+0xB)

// GPIO P1
sfr     P1                  = 0x90;
sbit    P10                 = 0x90;
sbit    P11                 = 0x91;
sbit    P12                 = 0x92;
sbit    P13                 = 0x93;
sbit    P14                 = 0x94;
sbit    P15                 = 0x95;
sbit    P16                 = 0x96;
sbit    P17                 = 0x97;
// GPIO P1 PCON1 = 0x80;
#define P1_PU               *(unsigned char volatile pdata *)(P1_BASE+0x0)
#define P1_PD               *(unsigned char volatile pdata *)(P1_BASE+0x1)
#define P1_MD0              *(unsigned char volatile pdata *)(P1_BASE+0x2)
#define P1_MD1              *(unsigned char volatile pdata *)(P1_BASE+0x3)
#define P1_TRG0             *(unsigned char volatile pdata *)(P1_BASE+0x4)
#define P1_TRG1             *(unsigned char volatile pdata *)(P1_BASE+0x5)
#define P1_PND              *(unsigned char volatile pdata *)(P1_BASE+0x6)
#define P1_IMK              *(unsigned char volatile pdata *)(P1_BASE+0x7)
#define P1_AIOEN            *(unsigned char volatile pdata *)(P1_BASE+0x8)
#define P1_AIOEN1           *(unsigned char volatile pdata *)(P1_BASE+0xC)
#define P1_AIOEN2           *(unsigned char volatile pdata *)(P1_BASE+0xD)
#define P1_DRV0             *(unsigned char volatile pdata *)(0xD0+0x9)
#define P1_DRV1             *(unsigned char volatile pdata *)(IODRV_BASE+0x7)
#define P1_DRV2             *(unsigned char volatile pdata *)(IODRV_BASE+0x8)
#define P1_DRV3             *(unsigned char volatile pdata *)(IODRV_BASE+0x9)
#define P1_DRV4             *(unsigned char volatile pdata *)(IODRV_BASE+0xA)
#define P1_DRV5             *(unsigned char volatile pdata *)(IODRV_BASE+0xB)
#define P1_DRV6             *(unsigned char volatile pdata *)(IODRV_BASE+0xC)
#define P1_DRV7             *(unsigned char volatile pdata *)(IODRV_BASE+0xD)
#define P1_ODN              *(unsigned char volatile pdata *)(P1_BASE+0xA)
#define P1_ODP              *(unsigned char volatile pdata *)(P1_BASE+0xB)

// GPIO P2
sfr     P2                  = 0xA0;
sbit    P20                 = 0xA0;
sbit    P21                 = 0xA1;
sbit    P22                 = 0xA2;
sbit    P23                 = 0xA3;
sbit    P24                 = 0xA4;
sbit    P25                 = 0xA5;
sbit    P26                 = 0xA6;
sbit    P27                 = 0xA7;
// GPIO P1 PCON1 = 0x80;
#define P2_PU               *(unsigned char volatile pdata *)(P2_BASE+0x0)
#define P2_PD               *(unsigned char volatile pdata *)(P2_BASE+0x1)
#define P2_MD0              *(unsigned char volatile pdata *)(P2_BASE+0x2)
#define P2_MD1              *(unsigned char volatile pdata *)(P2_BASE+0x3)
#define P2_TRG0             *(unsigned char volatile pdata *)(P2_BASE+0x4)
#define P2_TRG1             *(unsigned char volatile pdata *)(P2_BASE+0x5)
#define P2_PND              *(unsigned char volatile pdata *)(P2_BASE+0x6)
#define P2_IMK              *(unsigned char volatile pdata *)(P2_BASE+0x7)
#define P2_AIOEN            *(unsigned char volatile pdata *)(P2_BASE+0x8)
#define P2_AIOEN1           *(unsigned char volatile pdata *)(P2_BASE+0xC)
#define P2_AIOEN2           *(unsigned char volatile pdata *)(P2_BASE+0xD)
#define P2_DRV0             *(unsigned char volatile pdata *)(0xE0+0x9)
#define P2_DRV1             *(unsigned char volatile pdata *)(IODRV_BASE+0xE)
#define P2_DRV2             *(unsigned char volatile pdata *)(IODRV_BASE+0xF)
#define P2_DRV3             *(unsigned char volatile pdata *)(IODRV_BASE+0x10)
#define P2_DRV4             *(unsigned char volatile pdata *)(IODRV_BASE+0x11)
#define P2_DRV5             *(unsigned char volatile pdata *)(IODRV_BASE+0x12)
#define P2_DRV6             *(unsigned char volatile pdata *)(IODRV_BASE+0x13)
#define P2_DRV7             *(unsigned char volatile pdata *)(IODRV_BASE+0x14)
#define P2_ODN              *(unsigned char volatile pdata *)(P2_BASE+0xA)
#define P2_ODP              *(unsigned char volatile pdata *)(P2_BASE+0xB)

// GPIO P3
sfr     P3                  = 0xB0;
sbit    P30                 = 0xB0;
sbit    P31                 = 0xB1;
// GPIO P3 PCON1 = 0x80;
#define P3_PU               *(unsigned char volatile pdata *)(P3_BASE+0x0)
#define P3_PD               *(unsigned char volatile pdata *)(P3_BASE+0x1)
#define P3_MD0              *(unsigned char volatile pdata *)(P3_BASE+0x2)
#define P3_MD1              *(unsigned char volatile pdata *)(P3_BASE+0x3)
#define P3_TRG0             *(unsigned char volatile pdata *)(P3_BASE+0x4)
#define P3_TRG1             *(unsigned char volatile pdata *)(P3_BASE+0x5)
#define P3_PND              *(unsigned char volatile pdata *)(P3_BASE+0x6)
#define P3_IMK              *(unsigned char volatile pdata *)(P3_BASE+0x7)
#define P3_AIOEN            *(unsigned char volatile pdata *)(P3_BASE+0x8)
#define P3_AIOEN1           *(unsigned char volatile pdata *)(P3_BASE+0xC)
#define P3_AIOEN2           *(unsigned char volatile pdata *)(P3_BASE+0xD)
#define P3_DRV0             *(unsigned char volatile pdata *)(0xF0+0x9)
#define P3_DRV1             *(unsigned char volatile pdata *)(IODRV_BASE+0x15)
#define P3_ODN              *(unsigned char volatile pdata *)(P3_BASE+0xA)
#define P3_ODP              *(unsigned char volatile pdata *)(P3_BASE+0xB)

// GPIO AF   PCON1 = 0x81;
#define P0_AF0              *(unsigned char volatile xdata *)(IOFUNC_BASE+0x0)
#define P1_AF0              *(unsigned char volatile xdata *)(IOFUNC_BASE+0x2)
#define P2_AF0              *(unsigned char volatile xdata *)(IOFUNC_BASE+0x4)
#define P3_AF0              *(unsigned char volatile xdata *)(IOFUNC_BASE+0x6)
#define FIN_S0              *(unsigned char volatile xdata *)(IOFUNC_BASE+0x8)
#define FIN_S1              *(unsigned char volatile xdata *)(IOFUNC_BASE+0x9)
#define FIN_S2              *(unsigned char volatile xdata *)(IOFUNC_BASE+0xA)
#define FIN_S3              *(unsigned char volatile xdata *)(IOFUNC_BASE+0xB)
#define FIN_S4              *(unsigned char volatile xdata *)(IOFUNC_BASE+0xC)
#define FIN_S5              *(unsigned char volatile xdata *)(IOFUNC_BASE+0xD)
#define FIN_S6              *(unsigned char volatile xdata *)(IOFUNC_BASE+0xE)
#define FIN_S7              *(unsigned char volatile xdata *)(IOFUNC_BASE+0xF)
#define FIN_S8              *(unsigned char volatile xdata *)(IOFUNC_BASE+0x10)
#define FIN_S9              *(unsigned char volatile xdata *)(IOFUNC_BASE+0x11)
#define FIN_S10             *(unsigned char volatile xdata *)(IOFUNC_BASE+0x12)
#define FIN_S11             *(unsigned char volatile xdata *)(IOFUNC_BASE+0x13)
#define FIN_S12             *(unsigned char volatile xdata *)(IOFUNC_BASE+0x14)
#define FIN_S13             *(unsigned char volatile xdata *)(IOFUNC_BASE+0x15)
#define FIN_S14             *(unsigned char volatile xdata *)(IOFUNC_BASE+0x16)
#define FIN_S15             *(unsigned char volatile xdata *)(IOFUNC_BASE+0x17)
#define FOUT_S00            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x18)
#define FOUT_S01            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x19)
#define FOUT_S02            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x1A)
#define FOUT_S03            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x1B)
#define FOUT_S04            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x1C)
#define FOUT_S05            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x1D)
#define FOUT_S06            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x1E)
#define FOUT_S07            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x1F)
#define FOUT_S10            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x20)
#define FOUT_S11            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x21)
#define FOUT_S12            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x22)
#define FOUT_S13            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x23)
#define FOUT_S14            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x24)
#define FOUT_S15            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x25)
#define FOUT_S16            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x26)
#define FOUT_S17            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x27)
#define FOUT_S20            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x28)
#define FOUT_S21            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x29)
#define FOUT_S22            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x2A)
#define FOUT_S23            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x2B)
#define FOUT_S24            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x2C)
#define FOUT_S25            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x2D)
#define FOUT_S26            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x2E)
#define FOUT_S27            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x2F)
#define FOUT_S30            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x30)
#define FOUT_S31            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x31)
#define FOUT_SEL            *(unsigned char volatile xdata *)(IOFUNC_BASE+0x32)

// SPI
sfr     SPI_CON             = 0xB9;
sfr     SPI_BAUD            = 0xC9;
sfr     SPI_STA             = 0xCA;
sfr     SPI_DATA            = 0xCB;

// UART
sfr     UART0_CON0          = 0xD2;
sfr     UART0_CON1          = 0xD3;
sfr     UART0_STA           = 0xD4;
sfr     UART0_BAUD0         = 0xD5;
sfr     UART0_BAUD1         = 0xD6;
sfr     UART0_DATA          = 0xD7;
sfr     UART1_CON0          = 0xF2;
sfr     UART1_CON1          = 0xF3;
sfr     UART1_STA           = 0xF4;
sfr     UART1_BAUD0         = 0xF5;
sfr     UART1_BAUD1         = 0xF6;
sfr     UART1_DATA          = 0xF7;
sfr     UART1_DMACON        = 0xFD;
sfr     UART1_DMAADRH       = 0xF9;
sfr     UART1_DMAADRL       = 0xFA;
sfr     UART1_DMALEN        = 0xFB;

// I2C
sfr     I2C_CON             = 0xCC;
sfr     I2C_DATA            = 0xCD;
sfr     I2C_ADR             = 0xCE;
sfr     I2C_STA             = 0xCF;

// LED  PCON1 = 0x80;
#define LED_SEGCONL         *(unsigned char volatile pdata *)(LED_BASE+0x0)
#define LED_SEGCONH         *(unsigned char volatile pdata *)(LED_BASE+0x1)
#define LED_COMCON          *(unsigned char volatile pdata *)(LED_BASE+0x2)
#define LED_CON             *(unsigned char volatile pdata *)(LED_BASE+0x3)
#define LED_TIMECON         *(unsigned char volatile pdata *)(LED_BASE+0x4)
#define LED_DMAADRL         *(unsigned char volatile pdata *)(LED_BASE+0x5)
#define LED_DMAADRH         *(unsigned char volatile pdata *)(LED_BASE+0x6)

// MDU
sfr     MDU_CTL0            = 0x88;
sbit    MDU_EN              = 0x88;
sfr     MDU_CTL1            = 0x89;
sfr     MDU_ADR0            = 0x8A;
sfr     MDU_ADR1            = 0x8B;
sfr     MDU_ADR2            = 0x8C;
sfr     MDU_STA             = 0xF1;

// TMR0  PCON1 = 0x80;
#define TMR0_CONL           *(unsigned char volatile pdata *)(TMR0_BASE+0x00)
#define TMR0_CONH           *(unsigned char volatile pdata *)(TMR0_BASE+0x01)
#define TMR0_CNTL           *(unsigned char volatile pdata *)(TMR0_BASE+0x02)
#define TMR0_CNTH           *(unsigned char volatile pdata *)(TMR0_BASE+0x03)
#define TMR0_PRL            *(unsigned char volatile pdata *)(TMR0_BASE+0x04)
#define TMR0_PRH            *(unsigned char volatile pdata *)(TMR0_BASE+0x05)
#define TMR0_PWML           *(unsigned char volatile pdata *)(TMR0_BASE+0x06)
#define TMR0_PWMH           *(unsigned char volatile pdata *)(TMR0_BASE+0x07)
#define TMR0_PWML1          *(unsigned char volatile pdata *)(TMR0_BASE+0x0A)
#define TMR0_PWMH1          *(unsigned char volatile pdata *)(TMR0_BASE+0x0B)

// TMR1  PCON1 = 0x80;
#define TMR1_CONL           *(unsigned char volatile pdata *)(TMR1_BASE+0x00)
#define TMR1_CONH           *(unsigned char volatile pdata *)(TMR1_BASE+0x01)
#define TMR1_CNTL           *(unsigned char volatile pdata *)(TMR1_BASE+0x02)
#define TMR1_CNTH           *(unsigned char volatile pdata *)(TMR1_BASE+0x03)
#define TMR1_PRL            *(unsigned char volatile pdata *)(TMR1_BASE+0x04)
#define TMR1_PRH            *(unsigned char volatile pdata *)(TMR1_BASE+0x05)
#define TMR1_PWML           *(unsigned char volatile pdata *)(TMR1_BASE+0x06)
#define TMR1_PWMH           *(unsigned char volatile pdata *)(TMR1_BASE+0x07)
#define TMR1_PWML1          *(unsigned char volatile pdata *)(TMR1_BASE+0x0A)
#define TMR1_PWMH1          *(unsigned char volatile pdata *)(TMR1_BASE+0x0B)

// TMR2  PCON1 = 0x81;
#define TMR2_CONL           *(unsigned char volatile xdata *)(TMR2_BASE+0x0)
#define TMR2_CONH           *(unsigned char volatile xdata *)(TMR2_BASE+0x1)
#define TMR2_CNTL           *(unsigned char volatile xdata *)(TMR2_BASE+0x2)
#define TMR2_CNTH           *(unsigned char volatile xdata *)(TMR2_BASE+0x3)
#define TMR2_PRL            *(unsigned char volatile xdata *)(TMR2_BASE+0x4)
#define TMR2_PRH            *(unsigned char volatile xdata *)(TMR2_BASE+0x5)
#define TMR2_PWML           *(unsigned char volatile xdata *)(TMR2_BASE+0x6)
#define TMR2_PWMH           *(unsigned char volatile xdata *)(TMR2_BASE+0x7)

// TMR3  PCON1 = 0x81;
#define TMR3_CONL           *(unsigned char volatile xdata *)(TMR3_BASE+0x0)
#define TMR3_CONH           *(unsigned char volatile xdata *)(TMR3_BASE+0x1)
#define TMR3_CNTL           *(unsigned char volatile xdata *)(TMR3_BASE+0x2)
#define TMR3_CNTH           *(unsigned char volatile xdata *)(TMR3_BASE+0x3)
#define TMR3_PRL            *(unsigned char volatile xdata *)(TMR3_BASE+0x4)
#define TMR3_PRH            *(unsigned char volatile xdata *)(TMR3_BASE+0x5)
#define TMR3_PWML           *(unsigned char volatile xdata *)(TMR3_BASE+0x6)
#define TMR3_PWMH           *(unsigned char volatile xdata *)(TMR3_BASE+0x7)

// WUT  PCON1 = 0x81;
#define WUT_CONL            *(unsigned char volatile xdata *)(WUT_BASE+0x0)
#define WUT_CONH            *(unsigned char volatile xdata *)(WUT_BASE+0x1)
#define WUT_CNTL            *(unsigned char volatile xdata *)(WUT_BASE+0x2)
#define WUT_CNTH            *(unsigned char volatile xdata *)(WUT_BASE+0x3)
#define WUT_PRL             *(unsigned char volatile xdata *)(WUT_BASE+0x4)
#define WUT_PRH             *(unsigned char volatile xdata *)(WUT_BASE+0x5)
#define WUT_PWML            *(unsigned char volatile xdata *)(WUT_BASE+0x6)
#define WUT_PWMH            *(unsigned char volatile xdata *)(WUT_BASE+0x7)

// BUZ  PCON1 = 0x81;
#define BUZ_CON             *(unsigned char volatile xdata *)(BUZ_BASE+0x0)
#define BUZ_DIV             *(unsigned char volatile xdata *)(BUZ_BASE+0x1)

// TMR4
sfr     TMR4_CON0           = 0xDA;
sfr     TMR4_CON1           = 0xDB;
sfr     TMR4_CON2           = 0xDC;
sfr     TMR4_CON3           = 0xDD;
sfr     TMR4_EN             = 0xDE;
sfr     TMR4_IE0            = 0xDF;
sfr     TMR4_CLR0           = 0xE2;
sfr     TMR4_CNT0           = 0xE4;
sfr     TMR4_CNT1           = 0xE5;
sfr     TMR4_CAP10          = 0xE6;
sfr     TMR4_CAP11          = 0xE7;
sfr     TMR4_CAP20          = 0xE8;
sfr     TMR4_CAP21          = 0xE9;
sfr     TMR4_CAP30          = 0xEA;
sfr     TMR4_CAP31          = 0xEB;
sfr     TMR4_CAP40          = 0xEC;
sfr     TMR4_CAP41          = 0xED;
sfr     TMR4_FLAG0          = 0xEE;
sfr     TMR_ALLCON          = 0xD1;

// STMR  PCON1 = 0x80;
#define STMR_CON0           *(unsigned char volatile pdata *)(STMR_BASE+0x2)
#define STMR_CNTMD          *(unsigned char volatile pdata *)(STMR_BASE+0x3)
#define STMR_CNTCLR         *(unsigned char volatile pdata *)(STMR_BASE+0x4)
#define STMR_CNTTYPE        *(unsigned char volatile pdata *)(STMR_BASE+0x5)
#define STMR_CNTEN          *(unsigned char volatile pdata *)(STMR_BASE+0x6)
#define STMR_LOADEN         *(unsigned char volatile pdata *)(STMR_BASE+0x9)
#define STMR_CMPCL          *(unsigned char volatile pdata *)(STMR_BASE+0x7)
#define STMR_CMPCH          *(unsigned char volatile pdata *)(STMR_BASE+0x8)
#define STMR_PWMEN          *(unsigned char volatile pdata *)(STMR_BASE+0xA)
#define STMR_PWMVALA        *(unsigned char volatile pdata *)(STMR_BASE+0xB)
#define STMR_PWMVALB        *(unsigned char volatile pdata *)(STMR_BASE+0xC)
#define STMR_PWMBEN         *(unsigned char volatile pdata *)(STMR_BASE+0xD)
#define STMR_PWMMSKEN       *(unsigned char volatile pdata *)(STMR_BASE+0xE)
#define STMR_PWMMSKD        *(unsigned char volatile pdata *)(STMR_BASE+0xF)
#define STMR_BRKEN          *(unsigned char volatile pdata *)(STMR_BASE+0x10)
#define STMR_BRKDAT         *(unsigned char volatile pdata *)(STMR_BASE+0x11)
#define STMR_BRKCON         *(unsigned char volatile pdata *)(STMR_BASE+0x12)
#define STMR_BRKFILT        *(unsigned char volatile pdata *)(STMR_BASE+0x5A)
#define STMR01_DT           *(unsigned char volatile pdata *)(STMR_BASE+0x13)
#define STMR23_DT           *(unsigned char volatile pdata *)(STMR_BASE+0x14)
#define STMR45_DT           *(unsigned char volatile pdata *)(STMR_BASE+0x15)
#define STMR_DTCON          *(unsigned char volatile pdata *)(STMR_BASE+0x16)
#define STMR_DTEN           *(unsigned char volatile pdata *)(STMR_BASE+0x17)
#define STMR_EDGESEL        *(unsigned char volatile pdata *)(STMR_BASE+0x0)
#define STMR_DTDAT          *(unsigned char volatile pdata *)(STMR_BASE+0x1)
#define STMR0_IE            *(unsigned char volatile pdata *)(STMR_BASE+0x4E)
#define STMR1_IE            *(unsigned char volatile pdata *)(STMR_BASE+0x4F)
#define STMR2_IE            *(unsigned char volatile pdata *)(STMR_BASE+0x50)
#define STMR3_IE            *(unsigned char volatile pdata *)(STMR_BASE+0x51)
#define STMR4_IE            *(unsigned char volatile pdata *)(STMR_BASE+0x52)
#define STMR5_IE            *(unsigned char volatile pdata *)(STMR_BASE+0x53)
#define STMR0_IF            *(unsigned char volatile pdata *)(STMR_BASE+0x54)
#define STMR1_IF            *(unsigned char volatile pdata *)(STMR_BASE+0x55)
#define STMR2_IF            *(unsigned char volatile pdata *)(STMR_BASE+0x56)
#define STMR3_IF            *(unsigned char volatile pdata *)(STMR_BASE+0x57)
#define STMR4_IF            *(unsigned char volatile pdata *)(STMR_BASE+0x58)
#define STMR5_IF            *(unsigned char volatile pdata *)(STMR_BASE+0x59)
#define STMR0_PRL           *(unsigned char volatile pdata *)(STMR_BASE+0x18)
#define STMR0_PRH           *(unsigned char volatile pdata *)(STMR_BASE+0x19)
#define STMR1_PRL           *(unsigned char volatile pdata *)(STMR_BASE+0x1A)
#define STMR1_PRH           *(unsigned char volatile pdata *)(STMR_BASE+0x1B)
#define STMR2_PRL           *(unsigned char volatile pdata *)(STMR_BASE+0x1C)
#define STMR2_PRH           *(unsigned char volatile pdata *)(STMR_BASE+0x1D)
#define STMR3_PRL           *(unsigned char volatile pdata *)(STMR_BASE+0x1E)
#define STMR3_PRH           *(unsigned char volatile pdata *)(STMR_BASE+0x1F)
#define STMR4_PRL           *(unsigned char volatile pdata *)(STMR_BASE+0x20)
#define STMR4_PRH           *(unsigned char volatile pdata *)(STMR_BASE+0x21)
#define STMR5_PRL           *(unsigned char volatile pdata *)(STMR_BASE+0x22)
#define STMR5_PRH           *(unsigned char volatile pdata *)(STMR_BASE+0x23)
#define STMR0_CMPAL         *(unsigned char volatile pdata *)(STMR_BASE+0x24)
#define STMR0_CMPAH         *(unsigned char volatile pdata *)(STMR_BASE+0x25)
#define STMR0_CMPBL         *(unsigned char volatile pdata *)(STMR_BASE+0x26)
#define STMR0_CMPBH         *(unsigned char volatile pdata *)(STMR_BASE+0x27)
#define STMR1_CMPAL         *(unsigned char volatile pdata *)(STMR_BASE+0x28)
#define STMR1_CMPAH         *(unsigned char volatile pdata *)(STMR_BASE+0x29)
#define STMR1_CMPBL         *(unsigned char volatile pdata *)(STMR_BASE+0x2A)
#define STMR1_CMPBH         *(unsigned char volatile pdata *)(STMR_BASE+0x2B)
#define STMR2_CMPAL         *(unsigned char volatile pdata *)(STMR_BASE+0x2C)
#define STMR2_CMPAH         *(unsigned char volatile pdata *)(STMR_BASE+0x2D)
#define STMR2_CMPBL         *(unsigned char volatile pdata *)(STMR_BASE+0x2E)
#define STMR2_CMPBH         *(unsigned char volatile pdata *)(STMR_BASE+0x2F)
#define STMR3_CMPAL         *(unsigned char volatile pdata *)(STMR_BASE+0x30)
#define STMR3_CMPAH         *(unsigned char volatile pdata *)(STMR_BASE+0x31)
#define STMR3_CMPBL         *(unsigned char volatile pdata *)(STMR_BASE+0x32)
#define STMR3_CMPBH         *(unsigned char volatile pdata *)(STMR_BASE+0x33)
#define STMR4_CMPAL         *(unsigned char volatile pdata *)(STMR_BASE+0x34)
#define STMR4_CMPAH         *(unsigned char volatile pdata *)(STMR_BASE+0x35)
#define STMR4_CMPBL         *(unsigned char volatile pdata *)(STMR_BASE+0x36)
#define STMR4_CMPBH         *(unsigned char volatile pdata *)(STMR_BASE+0x37)
#define STMR5_CMPAL         *(unsigned char volatile pdata *)(STMR_BASE+0x38)
#define STMR5_CMPAH         *(unsigned char volatile pdata *)(STMR_BASE+0x39)
#define STMR5_CMPBL         *(unsigned char volatile pdata *)(STMR_BASE+0x3A)
#define STMR5_CMPBH         *(unsigned char volatile pdata *)(STMR_BASE+0x3B)
#define STMR0_CNTL          *(unsigned char volatile pdata *)(STMR_BASE+0x3C)
#define STMR0_CNTH          *(unsigned char volatile pdata *)(STMR_BASE+0x3D)
#define STMR1_CNTL          *(unsigned char volatile pdata *)(STMR_BASE+0x3E)
#define STMR1_CNTH          *(unsigned char volatile pdata *)(STMR_BASE+0x3F)
#define STMR2_CNTL          *(unsigned char volatile pdata *)(STMR_BASE+0x40)
#define STMR2_CNTH          *(unsigned char volatile pdata *)(STMR_BASE+0x41)
#define STMR3_CNTL          *(unsigned char volatile pdata *)(STMR_BASE+0x42)
#define STMR3_CNTH          *(unsigned char volatile pdata *)(STMR_BASE+0x43)
#define STMR4_CNTL          *(unsigned char volatile pdata *)(STMR_BASE+0x44)
#define STMR4_CNTH          *(unsigned char volatile pdata *)(STMR_BASE+0x45)
#define STMR5_CNTL          *(unsigned char volatile pdata *)(STMR_BASE+0x46)
#define STMR5_CNTH          *(unsigned char volatile pdata *)(STMR_BASE+0x47)
#define STMR0_PSC           *(unsigned char volatile pdata *)(STMR_BASE+0x48)
#define STMR1_PSC           *(unsigned char volatile pdata *)(STMR_BASE+0x49)
#define STMR2_PSC           *(unsigned char volatile pdata *)(STMR_BASE+0x4A)
#define STMR3_PSC           *(unsigned char volatile pdata *)(STMR_BASE+0x4B)
#define STMR4_PSC           *(unsigned char volatile pdata *)(STMR_BASE+0x4C)
#define STMR5_PSC           *(unsigned char volatile pdata *)(STMR_BASE+0x4D)

// CRC
sfr     CRC_CON             = 0xC1;
sfr     CRC_REG             = 0xC2;
sfr     CRC_FIFO            = 0xC3;
sfr     CRC_DATA0           = 0xC4;
sfr     CRC_DATA1           = 0xC5;
sfr     CRC_DATA2           = 0xC6;
sfr     CRC_DATA3           = 0xC7;

// FLASH
sfr     FLASH_CON           = 0xA4;
sfr     FLASH_STA           = 0xA5;
sfr     FLASH_DATA          = 0xA6;
sfr     FLASH_TIMEREG0      = 0xA7;
sfr     FLASH_TIMEREG1      = 0xAC;
sfr     FLASH_CRCLEN        = 0xAD;
sfr     FLASH_PASSWORD      = 0xAE;
sfr     FLASH_ADDR          = 0xAF;
sfr     FLASH_TRIM          = 0xBB;
sfr     FLASH_LOCK          = 0xC0;
sfr     FLASH_DMASTADR      = 0xBC;
sfr     FLASH_DMALEN        = 0xBD;
sfr     FLASH_BOOTCON       = 0xBE;
sfr     FLASH_ERRSTA        = 0xBF;
sfr     FLASH_FUNCON        = 0xFF;
// FLASH PCON1 = 0x81;
#define FLASH_DEBUGSTA      *(unsigned char volatile xdata *)(0x7199)

// ADC
sfr     ADC_CFG2            = 0x91;
sfr     ADC_CFG3            = 0x92;
sfr     ADC_CFG4            = 0xFC;
sfr     ADC_DATAH0          = 0x93;
sfr     ADC_DATAL0          = 0x94;
sfr     ADC_DATAH1          = 0x95;
sfr     ADC_DATAL1          = 0x96;
sfr     ADC_DATAH2          = 0x97;
sfr     ADC_DATAL2          = 0x98;
sfr     ADC_CHS0            = 0x99;
sfr     ADC_CHS1            = 0x9A;
sfr     ADC_CHS2            = 0x9D;
sfr     ADC_TRGS0           = 0x9E;
sfr     ADC_TRGS1           = 0x9F;
sfr     ADC_TRGS2           = 0xA1;
sfr     ADC_CMPDATAH        = 0xA2;
sfr     ADC_CMPDATAL        = 0xA3;
// ADC PCON1 = 0x81;
#define ADC_ACON0           *(unsigned char volatile xdata *)(AIPCON_BASE+0x2)
#define ADC_ACON1           *(unsigned char volatile xdata *)(AIPCON_BASE+0x3)
#define ADC_ACON2           *(unsigned char volatile xdata *)(AIPCON_BASE+0x4)
// ADC PCON1 = 0x80;
#define ADC_CFG0            *(unsigned char volatile pdata *)(ADC_BASE+0x00)
#define ADC_CFG1            *(unsigned char volatile pdata *)(ADC_BASE+0x01)
#define ADC_STA             *(unsigned char volatile pdata *)(ADC_BASE+0x02)

// CMP  PCON1 = 0x81;
sfr     CMP0_CON0           = 0xC8;
#define CMP0_CON1           *(unsigned char volatile xdata *)(CMP_BASE+0x3)
#define CMP0_CON2           *(unsigned char volatile xdata *)(CMP_BASE+0x4)
#define CMP0_CON3           *(unsigned char volatile xdata *)(CMP_BASE+0x5)
#define CMP0_CON4           *(unsigned char volatile xdata *)(CMP_BASE+0x6)
#define CMP0_CON5           *(unsigned char volatile xdata *)(CMP_BASE+0x9)
#define CMP0_DAC0           *(unsigned char volatile xdata *)(CMP_BASE+0x7)
#define CMP0_DAC1           *(unsigned char volatile xdata *)(CMP_BASE+0x8)
sfr     CMP1_CON0           = 0xF8;
#define CMP1_CON1           *(unsigned char volatile xdata *)(CMP_BASE+0xc)
#define CMP1_CON2           *(unsigned char volatile xdata *)(CMP_BASE+0xd)
#define CMP1_CON3           *(unsigned char volatile xdata *)(CMP_BASE+0xe)
#define CMP1_CON4           *(unsigned char volatile xdata *)(CMP_BASE+0xf)
#define CMP1_DAC0           *(unsigned char volatile xdata *)(CMP_BASE+0x10)
#define CMP1_DAC1           *(unsigned char volatile xdata *)(CMP_BASE+0x11)
#define CMP_CON             *(unsigned char volatile xdata *)(CMP_BASE+0x0)
#define CMP_STA             *(unsigned char volatile xdata *)(CMP_BASE+0x1)
#define CMP_HYCON           *(unsigned char volatile xdata *)(CMP_BASE+0xa)

// AMP  PCON1 = 0x81;
#define AMP_CON0            *(unsigned char volatile xdata *)(SYSCTRL0_BASE+0x5)
#define AMP_CON1            *(unsigned char volatile xdata *)(AIPCON_BASE+0x5)
#define AMP_CON2            *(unsigned char volatile xdata *)(AIPCON_BASE+0x6)
#define AMP_CON3            *(unsigned char volatile xdata *)(AIPCON_BASE+0x7)
#define AMP_CON4            *(unsigned char volatile xdata *)(AIPCON_BASE+0x8)
#define AMP_CON5            *(unsigned char volatile xdata *)(AIPCON_BASE+0x9)
#define AMP_CON6            *(unsigned char volatile xdata *)(AIPCON_BASE+0xA)
#define AMP_CON7            *(unsigned char volatile xdata *)(AIPCON_BASE+0xB)
#define AMP_CON8            *(unsigned char volatile xdata *)(AIPCON_BASE+0xC)
#define AMP_CON9            *(unsigned char volatile xdata *)(AIPCON_BASE+0xD)
#define AMP_CON10           *(unsigned char volatile xdata *)(AIPCON_BASE+0xE)
#define AMP_CON11           *(unsigned char volatile xdata *)(AIPCON_BASE+0xF)

// TK PCON1 = 0x80;
#define TK_APREDISCH        *(unsigned char volatile pdata *)(TK_BASE+0x00)
#define TK_APRECHARGE       *(unsigned char volatile pdata *)(TK_BASE+0x01)
#define TK_ACONVTIME        *(unsigned char volatile pdata *)(TK_BASE+0x02)
#define TK_BASEDIV0         *(unsigned char volatile pdata *)(TK_BASE+0x03)
#define TK_BASEDIV1         *(unsigned char volatile pdata *)(TK_BASE+0x04)
#define TK_BASEDIV2         *(unsigned char volatile pdata *)(TK_BASE+0x05)
#define TK_BASEDIV3         *(unsigned char volatile pdata *)(TK_BASE+0x06)
#define TK_ACON0            *(unsigned char volatile pdata *)(TK_BASE+0x07)
#define TK_ACON1            *(unsigned char volatile pdata *)(TK_BASE+0x08)
#define TK_ACON2            *(unsigned char volatile pdata *)(TK_BASE+0x09)
#define TK_ACON3            *(unsigned char volatile pdata *)(TK_BASE+0x0A)
#define TK_CHCON0           *(unsigned char volatile pdata *)(TK_BASE+0x0B)
#define TK_CHCON1           *(unsigned char volatile pdata *)(TK_BASE+0x0C)
#define TK_CHCON2           *(unsigned char volatile pdata *)(TK_BASE+0x0D)
#define TK_CHCON3           *(unsigned char volatile pdata *)(TK_BASE+0x0E)
#define TK_CON0             *(unsigned char volatile pdata *)(TK_BASE+0x0F)
#define TK_CON1             *(unsigned char volatile pdata *)(TK_BASE+0x10)
#define TK_CON2             *(unsigned char volatile pdata *)(TK_BASE+0x11)
#define TK_CON3             *(unsigned char volatile pdata *)(TK_BASE+0x12)
#define TK_CON4             *(unsigned char volatile pdata *)(TK_BASE+0x13)
#define TK_CON5             *(unsigned char volatile pdata *)(TK_BASE+0x18)
#define TK_CONVDATH         *(unsigned char volatile pdata *)(TK_BASE+0x14)
#define TK_CONVDATL         *(unsigned char volatile pdata *)(TK_BASE+0x15)
#define TK_DMACON0          *(unsigned char volatile pdata *)(TK_BASE+0x16)
#define TK_DMACON1          *(unsigned char volatile pdata *)(TK_BASE+0x17)
#define TK_PSRCNT           *(unsigned char volatile pdata *)(TK_BASE+0x19)
#define TK_PN16INIT0L       *(unsigned char volatile pdata *)(TK_BASE+0x1A)
#define TK_PN16INIT0H       *(unsigned char volatile pdata *)(TK_BASE+0x1B)
#define TK_PN16INIT1L       *(unsigned char volatile pdata *)(TK_BASE+0x1C)
#define TK_PN16INIT1H       *(unsigned char volatile pdata *)(TK_BASE+0x1D)

// CHIP  PCON1 = 0x80;
#define CHIP_IDL            *(unsigned char volatile pdata *)(CHIP_BASE+0x0)
#define CHIP_IDH            *(unsigned char volatile pdata *)(CHIP_BASE+0x1)
#define CHIP_DCN            *(unsigned char volatile pdata *)(CHIP_BASE+0x2)

// PCON1 = 0x81;
#define IO_MAP1             *(unsigned char volatile xdata *)(SYSCTRL0_BASE+0x1)
#define IR_CON              *(unsigned char volatile xdata *)(SYSCTRL0_BASE+0x3)

#endif // __TX8T326x_H__
