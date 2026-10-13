#include "pin_level_scan.h"
#include "instrument.h"

#include "user_config.h"

#if PIN_LEVEL_SCAN_ENABLE
void pin_level_scan_config(void)
{
    // N 挡检测脚：
    P1_MD0 &= ~(GPIO_P11_MODE_SEL(0x03)); // 输入模式

    // 1 档 检测脚：
    P1_MD0 &= ~(GPIO_P12_MODE_SEL(0x03)); // 输入模式

    // 2 档 检测脚：
    P1_MD1 &= ~(GPIO_P15_MODE_SEL(0x03)); // 输入模式

    // 3 档 检测脚：
    P1_MD1 &= ~(GPIO_P16_MODE_SEL(0x03)); // 输入模式

    // 4 档 检测脚：
    P3_MD0 &= ~(GPIO_P31_MODE_SEL(0x03)); // 输入模式

#if (!USER_DEBUG_ENABLE)
    // 5 档 检测脚：
    P2_MD1 &= ~(GPIO_P25_MODE_SEL(0x03)); // 输入模式
#endif

    // 6 档 检测脚：
    P1_MD1 &= ~(GPIO_P17_MODE_SEL(0x03)); // 输入模式

    // 左转向 检测脚
    P0_MD0 &= ~GPIO_P00_MODE_SEL(0x03);

    // 右转向 检测脚
    P0_MD1 &= ~GPIO_P07_MODE_SEL(0x03);

    // 近光灯 检测脚
    P0_MD1 &= ~GPIO_P04_MODE_SEL(0x03);

    // 大灯（远光灯） 检测脚
    P0_MD1 &= ~GPIO_P06_MODE_SEL(0x03);

    // 发动机故障 检测脚
    P0_MD1 &= ~GPIO_P05_MODE_SEL(0x03);

    // 电池检测脚
    P0_MD0 &= ~GPIO_P03_MODE_SEL(0x03);
}

// 引脚电平扫描
void pin_level_scan(void)
{
    // 扫描挡位时，以检测到的最低挡位为优先
    if (SIGNAL_VALID_LEV_OF_GEAR_N == PIN_DETECT_GEAR_N) {
        instrument.gear = GEAR_NEUTRAL;
    } else if (SIGNAL_VALID_LEV_OF_GEAR_1 == PIN_DETECT_GEAR_1) {
        instrument.gear = GEAR_FIRST;
    } else if (SIGNAL_VALID_LEV_OF_GEAR_2 == PIN_DETECT_GEAR_2) {
        instrument.gear = GEAR_SECOND;
    } else if (SIGNAL_VALID_LEV_OF_GEAR_3 == PIN_DETECT_GEAR_3) {
        instrument.gear = GEAR_THIRD;
#if !USER_DEBUG_ENABLE
        // 测试时，5挡检测脚用作打印，4挡和6挡用作HCK和HDA
    } else if (SIGNAL_VALID_LEV_OF_GEAR_4 == PIN_DETECT_GEAR_4) {
        instrument.gear = GEAR_FOURTH;
    } else if (SIGNAL_VALID_LEV_OF_GEAR_5 == PIN_DETECT_GEAR_5) {
        instrument.gear = GEAR_FIFTH;

    } else if (SIGNAL_VALID_LEV_OF_GEAR_6 == PIN_DETECT_GEAR_6) {
        instrument.gear = GEAR_SIXTH;
#endif
    } else {
        // 如果 N档 、 1档 ~ 6档 都没有检测到，说明挡位没有接线
        instrument.gear = GEAR_UNKNOWN;
    }

#if USER_DEBUG_ENABLE
    // printf("instrument.gear == %u\n", (u16)instrument.gear);
#endif

    if (SIGNAL_VALID_LEV_OF_LEFT_TURN == PIN_DETECT_LEFT_TURN) {
        instrument.left_turn_valid = 1;
    } else {
        instrument.left_turn_valid = 0;
    }

#if USER_DEBUG_ENABLE
    // printf("instrument.left_turn_valid == %u\n",
    //        (u16)instrument.left_turn_valid);
#endif

    if (SIGNAL_VALID_LEV_OF_RIGHT_TURN == PIN_DETECT_RIGHT_TURN) {
        instrument.right_turn_valid = 1;
    } else {
        instrument.right_turn_valid = 0;
    }

#if USER_DEBUG_ENABLE
    // printf("instrument.right_turn_valid == %u\n",
    //        (u16)instrument.right_turn_valid);
#endif

    if (SIGNAL_VALID_LEV_OF_LOW_BEAM == PIN_DETECT_LOW_BEAM) {
        instrument.low_beam_valid = 1;
    } else {
        instrument.low_beam_valid = 0;
    }

#if USER_DEBUG_ENABLE
    // printf("instrument.low_beam_valid == %u\n", (u16)instrument.low_beam_valid);
#endif

    if (SIGNAL_VALID_LEV_OF_HIGH_BEAM == PIN_DETECT_HIGH_BEAM) {
        instrument.high_beam_valid = 1;
    } else {
        instrument.high_beam_valid = 0;
    }

#if USER_DEBUG_ENABLE
    // printf("instrument.high_beam_valid == %u\n",
    //        (u16)instrument.high_beam_valid);
#endif

    if (SIGNAL_VALID_LEV_OF_BAT == PIN_DETECT_BAT) {
        instrument.bat_valid = 1;
    } else {
        instrument.bat_valid = 0;
    }
}

#endif
