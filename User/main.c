/**
 ******************************************************************************
 * @file    main.c
 * @author  HUGE-IC Application Team
 * @version V1.0.0
 * @date    05-11-2022
 * @brief   Main program body
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; COPYRIGHT 2021 HUGE-IC</center></h2>
 *
 * 版权说明后续补上
 *
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "include.h"
#include <string.h>
#include "user_config.h"

#include "aip1302.h"
#include "eeprom_24cxx.h"
#include "aip3368_driver.h"
#include "aip3368h_display.h"

#include "adc.h"

#include "tmr1.h"
#include "tmr2.h"
#include "uart0.h"

#include "key_driver.h"
#include "io_key.h"

#include "pin_level_scan.h"

#include "speed_scan.h"
#include "battery.h"
#include "instrument.h"
#include "mileage.h"
#include "engine_speed_scan.h"
#include "fuel_capacity.h"

#if USER_DEBUG_ENABLE

/**
 * @brief 初始化调试引脚，观察引脚电平翻转
 *
 */
// void debug_pin_init(void)
// {
//     // P0_MD0 &= ~GPIO_P00_MODE_SEL(0x03);
//     // P0_MD0 |= GPIO_P00_MODE_SEL(0x01); // 输出模式
//     // FOUT_S00 = GPIO_FOUT_AF_FUNC;
//     // DEBUG_PIN = 0;
// }
#endif

void user_init(void)
{
#if USER_DEBUG_ENABLE
    uart0_init();
    printf("sys reset\n");
#endif

#if PIN_LEVEL_SCAN_ENABLE
    pin_level_scan_config();
#endif

#if IO_KEY_ENABLE
    io_key_config(); // 按键的配置
#endif

#if SPEED_SCAN_ENABLE
    speed_scan_config(); // 时速扫描的配置
#endif

#if ENGINE_SPEED_SCAN_ENABLE
    engine_speed_scan_config(); // 发动机转速扫描的配置
#endif

    // 这条初始化需要放在iic初始化之前,先将时钟ic的片选拉低
    aip1302_config(); // 时钟IC
    // TEST ONLY 单独测试eeprom时,确保时钟IC的片选拉低
    // // CE脚
    // P2_MD0 &= ~GPIO_P21_MODE_SEL(0x3); // 清空配置
    // P2_MD0 |= GPIO_P21_MODE_SEL(0x1);  // 输出模式
    // FOUT_S21 = GPIO_FOUT_AF_FUNC;
    // P21 = 0;
    eeprom_24cxx_config();

    // led供电控制脚
    P1_MD0 &= ~GPIO_P10_MODE_SEL(0x03); // 清空配置
    P1_MD0 |= GPIO_P10_MODE_SEL(0x01);  // 输出模式
    FOUT_S10 = GPIO_FOUT_AF_FUNC;
    P10 = 1; // 使能led供电

    aip3368h_module_init();

#if (FUEL_CAPACITY_SCAN_ENABLE || BATTERY_SCAN_ENABLE)
    adc_config();
#endif

    // instrument_info_init(); // 初始化仪表信息

    tmr1_config(); //
    // tmr2_config(); // 扫描脉冲(电平变化)的定时器

    delay_ms(10); // 等待系统稳定（主要是等adc采集完成一轮数据）
}

void main(void)
{
    // 看门狗默认打开, 复位时间2s
    system_init();

    // 关闭HCK和HDA的调试功能
    WDT_KEY = 0x55; // 解除写保护
    // 清除这个寄存器的值，实现关闭HCK和HDA引脚的调试功能（解除映射）
    IO_MAP &= ~0x01;
    WDT_KEY = 0xBB;

    /* 用户代码初始化接口 */
    user_init();

    /* 系统主循环 */
    while (1) {
#if USER_DEBUG_ENABLE
        // printf("main circle\n");
#endif

        WDT_KEY = WDT_KEY_VAL(0xAA); // 喂狗并清除 wdt_pending

        // P10 = !P10;

#if PIN_LEVEL_SCAN_ENABLE
        pin_level_scan();
#endif

#if IO_KEY_ENABLE
        key_driver_scan(&io_key_para);
        io_key_handle(); // io按键处理函数
#endif

#if SPEED_SCAN_ENABLE
        speed_scan(); // 检测时速
#endif
#if 0
        mileage_scan(); // 检测大计里程和小计里程
#endif

#if FUEL_CAPACITY_SCAN_ENABLE
        fuel_capacity_scan(); // 油量检测
#endif

        // bat_scan();

#if ENGINE_SPEED_SCAN_ENABLE
        engine_speed_scan(); // 检测发动机转速
#endif

#if 0
        if (aip1302_update_time_interval >= AIP1302_UPDATE_TIME_INTERVAL) {
            aip1302_update_time_interval = 0;
            aip1302_read_all(); // 读取时间
            instrument.time_hour = aip1302_info.time_hour;
            instrument.time_minute = aip1302_info.time_min;

#if USER_DEBUG_ENABLE
            // printf("hour == %u\n", (u16)instrument.time_hour);
            // printf("min == %u\n", (u16)instrument.time_minute);
            // printf("aip1302: \n");
            // printf("hour == %u\n", (u16)aip1302_info.time_hour);
            // printf("min == %u\n", (u16)aip1302_info.time_min);
            // printf("sec == %u\n", (u16)aip1302_info.time_sec);
#endif
        }
 
        instrument_info_save_handle();

        instrument_info_report_handle();
#endif

        aip3368h_module_display();

// TEST ONLY
#if AIP1302_TEST_ENABLE
        aip1302_test();
#endif
#if EEPROM_24CXX_TEST_ENABLE
        test_eeprom_24cxx();
#endif
    }
}

/**
 * @}
 */

/*************************** (C) COPYRIGHT 2022 HUGE-IC ***** END OF FILE *****/
