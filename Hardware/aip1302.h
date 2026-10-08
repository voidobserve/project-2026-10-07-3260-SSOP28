#ifndef __AIP1302_H__
#define __AIP1302_H__

#include "include.h" // 使用芯片官方提供的头文件

#define IC_1302_ENABLE      1
#define AIP1302_TEST_ENABLE 1 // TEST ONLY

// 每隔多久，从时钟IC读取一次时间，更新显示
#define AIP1302_UPDATE_TIME_INTERVAL 1000 // 单位 ms

#if IC_1302_ENABLE

typedef struct
{
    // 日期 4byte 年 月 日
    u16 year; // 年份
    u8 month; // 月份
    u8 day;   // 日期

    // 时间 3byte 时 分 秒，24H制存放
    u8 time_hour;
    u8 time_min;
    u8 time_sec;
} aip1302_info_t;

#define AIP1302_CE_PIN   P21
#define AIP1302_SCLK_PIN P22
#define AIP1302_DATA_PIN P23

// 写地址(读地址是在写地址的基础上加一)
#define AIP1302_SEC_REG_ADDR           ((u8)0x80) // 秒寄存器地址
#define AIP1302_MIN_REG_ADDR           ((u8)0x82) // 分寄存器地址
#define AIP1302_HOUR_REG_ADDR          ((u8)0x84) // 小时寄存器地址
#define AIP1302_DATE_REG_ADDR          ((u8)0x86) // 日
#define AIP1302_MONTH_REG_ADDR         ((u8)0x88) // 月
#define AIP1302_WEEKDAY_REG_ADDR       ((u8)0x8A) // 星期
#define AIP1302_YEAR_REG_ADDR          ((u8)0x8C) // 年
#define AIP1302_WRITE_PROTECT_REG_ADDR ((u8)0x8E) // 写保护位读地址

#define AIP1302_RAM_START_ADDR ((u8)0xC0) // RAM起始地址

// 48MHz 主频下 delay(4) 仅约 150~350ns，早于 DS1302 数据输出延迟 tCDD(最大500ns)，
// 会导致采样过早、读出固定的交替值(如秒=85即0x55)。加大延时保证电平稳定后再采样。
#define AIP1302_DELAY() delay(5)

extern volatile aip1302_info_t aip1302_info;
extern volatile u16 aip1302_update_time_interval;

void aip1302_config(void);
u8 aip1302_read_byte(const u8 cmd);
void aip1302_write_byte(const u8 cmd, u8 byte);
// 向aip1302更新时间，时:分:秒
void aip1302_update_time(aip1302_info_t aip1302_info);
#if 0
void aip1302_update_date(aip1302_info_t aip1302_info);     // 向aip1302更新日期,年月日
#endif
// void aip1302_update_weekday(aip1302_info_t aip1302_info);  // 向aip1302更新星期

void aip1302_read_all(void); // 从aip1302中读出所有有关时间的数据

#if AIP1302_TEST_ENABLE
void aip1302_test_1ms_isr(void);
void aip1302_test(void);
#endif
#if 0
// void aip1302_update_all_data(aip1302_info_t aip1302_info); // 从aip1302中读出所有有关时间的数据,存放到全局变量中
#endif

#endif

#endif
