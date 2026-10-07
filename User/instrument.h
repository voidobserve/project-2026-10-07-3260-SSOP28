#ifndef __INSTRUMENT_H__
#define __INSTRUMENT_H__

#include "typedef.h"

// 延时写入flash的时间：
#define INSTRUMENT_INFO_SAVE_TIME ((u16)2000)
// 发送一帧有关仪表数据的时间间隔
#define INSTRUMENT_INFO_REPORT_PERIOD ((u8)50)

// 挡位的定义
enum
{
    GEAR_NEUTRAL = 0x00, // 空挡

    GEAR_FIRST = 0x01,  // 一档
    GEAR_SECOND = 0x02, // 二档
    GEAR_THIRD = 0x03,  // 三档
    GEAR_FOURTH = 0x04, // 四档
    GEAR_FIFTH = 0x05,  // 五档
    GEAR_SIXTH = 0x06,  // 六档
    /*
        未知，如果 GEAR_NEUTRAL ~ GEAR_SIXTH 都没有检测到，
        则返回 GEAR_UNKNOWN ， 让显示屏中档位对应的图标空着
    */
    GEAR_UNKNOWN = 0xFF,
};
typedef u8 gear_t;

// 定义存储在flash中的数据
typedef struct
{
    // 总里程表（单位：m，使用英制单位时，只需要再发送时进行转换）
    // （大计里程，范围：0 ~ xxx KM）
    u32 total_mileage;
    // 短距离里程表(单位：m，使用英制单位时，只需要再发送时进行转换)
    // （小计里程，范围：0 ~ xxx KM）
    u32 subtotal_mileage;

    u8 is_save_data_valid;
} save_info_t;

typedef struct
{
    save_info_t save_info;
    u32 engine_speed; // 发动机的转速（单位：rpm）

    u16 speed; // 时速(单位：km/h，使用英制单位时，需要进行转换)

    u8 fuel_percent; // 油量(单位：百分比)

    gear_t gear;     // 档位
    u16 bat_voltage; // 电池电压（单位：mV）
    u8 time_hour;    // 小时
    u8 time_minute;  // 分钟

    u8 left_turn_valid;         // 左转灯是否有效
    u8 right_turn_valid;        // 右转灯是否有效
    u8 temp_of_water_err_valid; // 水温异常提示是否有效
    u8 high_beam_valid;         // 远光灯是否有效

    u8 key_event; // 按键事件
} instrument_t;
extern volatile instrument_t instrument;

enum
{
    REPORT_IDX_GEAR = 0,
    REPORT_IDX_BAT_VOLTAGE,
    REPORT_IDX_TIME,
    REPORT_IDX_ENGINE_SPEED,
    REPORT_IDX_SPEED,
    REPORT_IDX_TOTAL_MILEAGE,
    REPORT_IDX_SUBTOTAL_MILEAGE,
    REPORT_IDX_LEFT_TURN,
    REPORT_IDX_RIGHT_TURN,
    REPORT_IDX_TEMP_OF_WATER_ERR,
    REPORT_IDX_HIGH_BEAM,
    REPORT_IDX_FUEL_PERCENT,

    REPORT_IDX_MAX,
};

void instrument_info_init(void);
void instrument_info_save(void);

void instrument_info_save_time_add(void);
void instrument_info_save_enable(void);
void instrument_info_save_handle(void);

#endif
