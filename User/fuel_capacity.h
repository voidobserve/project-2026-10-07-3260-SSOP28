#ifndef __FUEL_CAPACITY_H__
#define __FUEL_CAPACITY_H__

#include "include.h"     // 使用芯片官方提供的头文件
#include "user_config.h" // 包含自定义的头文件

#if FUEL_CAPACITY_SCAN_ENABLE

// 选用油量检测对应的通道时，adc的参考电压，单位：mV
#define ADC_REF_OF_FUEL ((u16)3000)

// 将采集到的ad值转换成检测点对应的电压值，单位：mV
#define FUEL_ADC_VAL_TO_VOLTAGE(adc_val)                                       \
    ((u16)((u32)(adc_val) * ADC_REF_OF_FUEL / 4095))

// 油量检测脚的上拉与上拉电源（两个 200Ω 并联 = 100Ω 上拉到 ACC_5V，见 芯片引脚和功能.md）
#define FUEL_PULLUP_X10 ((u32)1000) // 上拉电阻，单位：0.1Ω
#define FUEL_RAIL_MV    ((u32)5000) // 上拉电源 (ACC_5V) 标称值，单位：mV

// 满油量对应的电压，单位：mV（容差安全窗口的低端，见文档“容差与误差预算”）
// 初值按 ACC_5V ±5%、上拉 ±1%、传感器端值 ±3Ω 推得，需按实测标定后替换：
// #define FUEL_FULL_VOLTAGE ((u16)788) // 旧标定值（等效传感器 18.7Ω），保留备查
#define FUEL_FULL_VOLTAGE ((u16)610)

// 没有油量对应的电压，单位：mV（容差安全窗口的高端）
// #define FUEL_EMPTY_VOLTAGE ((u16)1448) // 旧标定值（等效传感器 40.8Ω），保留备查
#define FUEL_EMPTY_VOLTAGE ((u16)2240)

// 检测脚电压(mV) -> 传感器等效下拉电阻(0.1Ω)：R = Rpu * V / (Vrail - V)
// 只可在 FUEL_FULL_VOLTAGE ~ FUEL_EMPTY_VOLTAGE 区间内调用，避免 V >= Vrail 时除零
#define FUEL_NODE_VOLTAGE_TO_R_X10(voltage)                                    \
    ((u32)FUEL_PULLUP_X10 * (u32)(voltage) / (FUEL_RAIL_MV - (u32)(voltage)))

#define FUEL_FULL_R_X10  FUEL_NODE_VOLTAGE_TO_R_X10(FUEL_FULL_VOLTAGE)
#define FUEL_EMPTY_R_X10 FUEL_NODE_VOLTAGE_TO_R_X10(FUEL_EMPTY_VOLTAGE)

// 每次计算油量的时间周期，单位：ms
#define FUEL_SCAN_PERIOD ((u8)200)

void fuel_capacity_scan_time_add(void);
void fuel_capacity_adc_val_samples_update(u16 adc_val);
u16 fuel_capacity_adc_val_get(void);

void fuel_capacity_scan(void);

#endif //
#endif
