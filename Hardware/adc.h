#ifndef __ADC_H__
#define __ADC_H__

#include "include.h" // 使用芯片官方提供的头文件
#include "user_config.h"

// 定义adc检测引脚
// ADC_PIN //
enum
{
    ADC_CHANNEL_NONE = 0x00,
    ADC_CHANNEL_FUEL,    // 检测油量
    ADC_CHANNEL_BATTERY, // 电池电量检测
};
typedef u8 adc_channel_t;

// 定义adc通道的各个状态，供状态机使用
enum
{
    ADC_CHANNEL_STATUS_NONE = 0,

    ADC_CHANNEL_STATUS_SEL_FUEL_BEGIN, // 检测油量的通道
    ADC_CHANNEL_STATUS_SEL_FUEL_END,

    ADC_CHANNEL_STATUS_SEL_BATTERY_BEGIN, // 选择了检测电池的通道，等待adc稳定
    ADC_CHANNEL_STATUS_SEL_BATTERY_END, // 选择了检测电池的通道，已经等待adc稳定
};
typedef u8 adc_channel_status_sel_t;

extern u16 adc_val;

void adc_config(void);
void adc_channel_set(adc_channel_t adc_channel);

// u16 adc_single_convert(void);

void adc_channel_switch_by_isr(void);

#endif
