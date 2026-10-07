#ifndef __FUEL_CAPACITY_H__
#define __FUEL_CAPACITY_H__

#include "include.h"     // 使用芯片官方提供的头文件
#include "user_config.h" // 包含自定义的头文件

#if FUEL_CAPACITY_SCAN_ENABLE

// 选用油量检测对应的通道时，adc的参考电压，单位：mV
#define ADC_REF_OF_FUEL ((u16)2000)

// 将采集到的ad值转换成检测点对应的电压值，单位：mV
#define FUEL_ADC_VAL_TO_VOLTAGE(adc_val)                                       \
    ((u16)((u32)(adc_val) * ADC_REF_OF_FUEL / 4095))

// 满油量对应的电压，单位：mV
#define FUEL_FULL_VOLTAGE ((u16)788)
// 没有油量对应的电压，单位：mV
#define FUEL_EMPTY_VOLTAGE ((u16)1448)

// 每次计算油量的时间周期，单位：ms
#define FUEL_SCAN_PERIOD ((u16)200)

void fuel_capacity_scan_time_add(void);
void fuel_capacity_adc_val_samples_update(u16 adc_val);
u16 fuel_capacity_adc_val_get(void);

void fuel_capacity_scan(void);

#endif //
#endif
