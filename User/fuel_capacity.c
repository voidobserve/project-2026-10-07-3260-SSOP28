#include "fuel_capacity.h"
#include "instrument.h"

#include "user_config.h"
// #include "aip3368h_display.h"

#if FUEL_CAPACITY_SCAN_ENABLE

static volatile u8 fuel_capacity_scan_time_cnt = 0;
static volatile u16 fuel_lev_update_time_cnt = 0;

// 油量ad值的滑动平均：
#define SAMPLE_COUNT 20 // 样本计数
static volatile u16 fuel_capacity_adc_val_samples[SAMPLE_COUNT] = {0};
static volatile u8 fuel_capacity_adc_val_sample_index = 0;

static void __fuel_capacity_adc_val_samples_init__(u16 adc_val)
{
    u8 i;
    for (i = 0; i < SAMPLE_COUNT; i++) {
        fuel_capacity_adc_val_samples[i] = adc_val;
    }
}

/**
 * @brief 将数据放入油量检测的滑动平均数组，由adc中断更新
 * 
 */
void fuel_capacity_adc_val_samples_update(u16 adc_val)
{
    static u8 is_initialized = 0;
    if (0 == is_initialized) {
        is_initialized = 1;
        __fuel_capacity_adc_val_samples_init__(adc_val);
        return; // 初始化数组之后，直接退出，下一次得到新数据才执行下面的操作
    }

    fuel_capacity_adc_val_samples[fuel_capacity_adc_val_sample_index] = adc_val;
    fuel_capacity_adc_val_sample_index++;
    if (fuel_capacity_adc_val_sample_index >= SAMPLE_COUNT) {
        fuel_capacity_adc_val_sample_index = 0;
    }
}

// 从油量检测的滑动平均数组中获取数据
u16 fuel_capacity_adc_val_get(void)
{
    u8 i;
    u32 sum = 0;
    for (i = 0; i < SAMPLE_COUNT; i++) {
        sum += fuel_capacity_adc_val_samples[i];
    }

    return (sum / SAMPLE_COUNT);
}

void fuel_capacity_scan_time_add(void)
{
    if (fuel_capacity_scan_time_cnt < ((u16)-1)) {
        fuel_capacity_scan_time_cnt++;
    }
}

// 滑动平均：
#define FUEL_VOLTAGE_SAMPLE_COUNT 20 // 样本计数
static volatile u16 fuel_voltage_samples[FUEL_VOLTAGE_SAMPLE_COUNT] = {0};
static volatile u8 fuel_voltage_sample_index = 0;
void __fuel_voltage_samples_init__(u16 voltage)
{
    u8 i;
    for (i = 0; i < FUEL_VOLTAGE_SAMPLE_COUNT; i++) {
        fuel_voltage_samples[i] = voltage;
    }
}

void __fuel_voltage_samples_update__(u16 voltage)
{
    fuel_voltage_samples[fuel_voltage_sample_index] = voltage;
    fuel_voltage_sample_index++;
    if (fuel_voltage_sample_index >= FUEL_VOLTAGE_SAMPLE_COUNT) {
        fuel_voltage_sample_index = 0;
    }
}

u16 __fuel_voltage_samples_get__(void)
{
    u8 i;
    u32 sum = 0;
    for (i = 0; i < FUEL_VOLTAGE_SAMPLE_COUNT; i++) {
        sum += fuel_voltage_samples[i];
    }

    return (sum / FUEL_VOLTAGE_SAMPLE_COUNT);
}

/**
 * @brief 将油量检测点的电压转换成对应的百分比值
 * 
 * @param voltage 
 * @return u8 
 */
u8 fuel_convert_voltage_to_percent(u16 voltage)
{
    u8 percent;
    if (voltage <= FUEL_FULL_VOLTAGE) {
        percent = 100;
    } else if (voltage >= FUEL_EMPTY_VOLTAGE) {
        percent = 0;
    } else {
        // 在 0 ~ 100 % 之间，线性划分
        percent = (100 - ((u32)FUEL_EMPTY_VOLTAGE - voltage) * 100 /
                             (FUEL_EMPTY_VOLTAGE - FUEL_FULL_VOLTAGE));
    }

    return percent;
}

void fuel_capacity_scan(void)
{
    u16 fuel_adc_val; // 油量检测脚采集的ad值
    u16 fuel_voltage; // 油量检测脚采集的电压值
    u8 fuel_percent;

    static u8 is_initialized = 0; // 初始化

    if (fuel_capacity_scan_time_cnt >= FUEL_SCAN_PERIOD) {
        fuel_capacity_scan_time_cnt = 0;
    } else {
        // 没有到扫描时间周期，直接返回
        return;
    }

    // 获取ad值
    fuel_adc_val = fuel_capacity_adc_val_get();
    // 根据油量检测的ADC值，计算出对应的检测点的电压值
    fuel_voltage = FUEL_ADC_VAL_TO_VOLTAGE(fuel_adc_val);
#if USER_DEBUG_ENABLE
    // printf("fuel_adc_val == %u\n", fuel_adc_val);
    // printf("fuel_voltage == %u\n", fuel_voltage);
#endif

    if (is_initialized == 0) {
        is_initialized = 1;
        __fuel_voltage_samples_init__(fuel_voltage);
    } else {
        __fuel_voltage_samples_update__(fuel_voltage);
    }

    fuel_voltage = __fuel_voltage_samples_get__();
    fuel_percent = fuel_convert_voltage_to_percent(fuel_voltage);
    instrument.fuel_percent = fuel_percent;
#if USER_DEBUG_ENABLE
    // printf("fuel_voltage == %u\n", fuel_voltage);
    // printf("fuel_percent == %u\n", (u16)fuel_percent);
#endif
}

#endif
