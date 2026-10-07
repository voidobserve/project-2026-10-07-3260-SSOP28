#include "pin_level_scan.h"
// #include "aip3368h_display.h"
#include "instrument.h"

#include "user_config.h"

#if PIN_LEVEL_SCAN_ENABLE
void pin_level_scan_config(void)
{
    // N 挡检测脚：
    P0_MD1 &= ~(GPIO_P06_MODE_SEL(0x03)); // 输入模式

    // 1 档 检测脚：
    P0_MD1 &= ~(GPIO_P07_MODE_SEL(0x03)); // 输入模式

    // 2 档 检测脚：
    P1_MD0 &= ~(GPIO_P10_MODE_SEL(0x03)); // 输入模式

    // 3 档 检测脚：
    P1_MD0 &= ~(GPIO_P13_MODE_SEL(0x03)); // 输入模式

    // 4 档 检测脚：
    P1_MD1 &= ~(GPIO_P14_MODE_SEL(0x03)); // 输入模式

    // 5 档 检测脚：
    P3_MD0 &= ~(GPIO_P30_MODE_SEL(0x03)); // 输入模式

    // 6 档 检测脚：
    P2_MD1 &= ~(GPIO_P27_MODE_SEL(0x03)); // 输入模式

    // 大灯（远光灯） 检测脚
    P2_MD0 &= ~GPIO_P21_MODE_SEL(0x03);

    // 左转向 检测脚
    P2_MD1 &= ~GPIO_P25_MODE_SEL(0x03);

    // 右转向 检测脚
    P2_MD0 &= ~GPIO_P22_MODE_SEL(0x03);

    // 水温异常检测脚
    P2_MD0 &= ~GPIO_P20_MODE_SEL(0x03);
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
    } else if (SIGNAL_VALID_LEV_OF_GEAR_4 == PIN_DETECT_GEAR_4) {
        instrument.gear = GEAR_FOURTH;
    } else if (SIGNAL_VALID_LEV_OF_GEAR_5 == PIN_DETECT_GEAR_5) {
        instrument.gear = GEAR_FIFTH;
    } else if (SIGNAL_VALID_LEV_OF_GEAR_6 == PIN_DETECT_GEAR_6) {
        instrument.gear = GEAR_SIXTH;
    } else {
        // 如果 N档 、 1档 ~ 6档 都没有检测到，说明挡位没有接线
        instrument.gear = GEAR_UNKNOWN;
    }

    if (SIGNAL_VALID_LEV_OF_LEFT_TURN == PIN_DETECT_LEFT_TURN) {
        instrument.left_turn_valid = 1;
    } else {
        instrument.left_turn_valid = 0;
    }

    if (SIGNAL_VALID_LEV_OF_RIGHT_TURN == PIN_DETECT_RIGHT_TURN) {
        instrument.right_turn_valid = 1;
    } else {
        instrument.right_turn_valid = 0;
    }

    if (SIGNAL_VALID_LEV_OF_HIGH_BEAM == PIN_DETECT_HIGH_BEAM) {
        instrument.high_beam_valid = 1;
    } else {
        instrument.high_beam_valid = 0;
    }

    if (SIGNAL_VALID_LEV_OF_TEMP_OF_WATER_ERR == PIN_DETECT_TEMP_OF_WATER_ERR) {
        instrument.temp_of_water_err_valid = 1;
    } else {
        instrument.temp_of_water_err_valid = 0;
    }
}

#endif
