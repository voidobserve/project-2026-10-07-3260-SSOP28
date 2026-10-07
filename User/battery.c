#include "battery.h"
#include "instrument.h"
#include "user_config.h"

#if BATTERY_SCAN_ENABLE

// 电池电压扫描时间计时（在定时器中累加）
static volatile u16 battery_scan_time_cnt = 0;

// 电池对应的ad值滑动平均：
#define SAMPLE_COUNT 20 // 样本计数
static volatile u16 bat_adc_val_samples[SAMPLE_COUNT] = {0};
static volatile u8 bat_adc_val_sample_index = 0;

// 电池电压值的滑动平均
#define BAT_VOL_SAMPLE_COUNT 20 // 样本计数
static volatile u16 bat_vol_samples[BAT_VOL_SAMPLE_COUNT] = {0};
static volatile u8 bat_vol_sample_index = 0;

// 初始化 滑动平均 数组
static void __bat_adv_val_samples_init__(u16 adc_val)
{
    u8 i;
    for (i = 0; i < SAMPLE_COUNT; i++) {
        bat_adc_val_samples[i] = adc_val;
    }
}

/**
 * @brief 将数据放入滑动平均数组，由adc中断更新
 *
 * @param adc_val
 *
 */
void bat_adc_val_samples_update(u16 adc_val)
{
    static u8 is_initialized = 0;
    if (0 == is_initialized) {
        is_initialized = 1;
        __bat_adv_val_samples_init__(adc_val);
        return; // 初始化数组之后，直接退出，下一次得到新数据才执行下面的操作
    }

    bat_adc_val_samples[bat_adc_val_sample_index] = adc_val;
    bat_adc_val_sample_index++;
    if (bat_adc_val_sample_index >= SAMPLE_COUNT) {
        bat_adc_val_sample_index = 0;
    }
}

// 从滑动平均数组中读出数据
u16 bat_adc_val_get_avg(void)
{
    u8 i;
    u32 sum = 0;
    for (i = 0; i < SAMPLE_COUNT; i++) {
        sum += bat_adc_val_samples[i];
    }

    return (sum / SAMPLE_COUNT);
}

// 初始化电池电压的滑动平均数组
void bat_vol_samples_init(u16 voltage)
{
    u8 i;
    for (i = 0; i < BAT_VOL_SAMPLE_COUNT; i++) {
        bat_vol_samples[i] = voltage;
    }
}

// 向电池电压的滑动平均数组中放入数据
void bat_vol_samples_update(u16 voltage)
{
    static u8 is_initialized = 0;
    if (0 == is_initialized) {
        is_initialized = 1;
        bat_vol_samples_init(voltage);
        return; // 初始化数组之后，直接退出，下一次得到新数据才执行下面的操作
    }

    bat_vol_samples[bat_vol_sample_index] = voltage;
    bat_vol_sample_index++;
    if (bat_vol_sample_index >= BAT_VOL_SAMPLE_COUNT) {
        bat_vol_sample_index = 0;
    }
}

// 从电池电压的滑动平均数组中取出平均值
u16 bat_vol_get_avg(void)
{
    u8 i;
    u32 sum = 0;
    for (i = 0; i < BAT_VOL_SAMPLE_COUNT; i++) {
        sum += bat_vol_samples[i];
    }

    return (sum / BAT_VOL_SAMPLE_COUNT);
}

void bat_scan_time_add(void)
{
    if (battery_scan_time_cnt < ((u16)-1)) {
        battery_scan_time_cnt++;
    }
}

void bat_scan(void)
{
    u16 adc_val;
    u16 voltage; // 存放电压值，单位：mV

    static u8 is_initialized = 0; // 是否初始化过

    if (battery_scan_time_cnt < BAT_SCAN_PERIOD) {
        return;
    }
    battery_scan_time_cnt = 0;

    adc_val = bat_adc_val_get_avg();
    voltage = ADC_VAL_TO_BAT_VOLTAGE(adc_val);

#if USER_DEBUG_ENABLE
// printf("adc_val == %u\n", adc_val);
// printf("voltage == %u\n", voltage);
#endif

    if (is_initialized == 0) {
        is_initialized = 1;
        bat_vol_samples_init(voltage); // 初始化电压值对应的滑动平均数组

    } else {
        // 如果已经初始化过，则更新电压值对应的滑动平均数组
        bat_vol_samples_update(voltage);
    }

    voltage = bat_vol_get_avg(); // 获取电压值的滑动平均
#if USER_DEBUG_ENABLE
    // printf("avg voltage == %u\n", voltage);
#endif

    instrument.bat_voltage = voltage;
}
#endif // BATTERY_SCAN_ENABLE