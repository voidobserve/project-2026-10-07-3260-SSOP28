#ifndef __INSTRUMENT_H__
#define __INSTRUMENT_H__

#include "typedef.h"

// 延时写入flash的时间：
#define INSTRUMENT_INFO_SAVE_TIME ((u16)2000)

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
    /*
        总里程表（单位：m）
        大计里程，范围：0 ~ 4,294,967.296 km，至少429万km
    */
    u32 total_mileage;
    /*
        短距离里程表(单位：m)
        小计里程，范围：0 ~ 4,294,967.296 km，至少429万km
    */
    u32 subtotal_mileage;

    // 大计里程反码(用于校验)
    u32 total_mileage_inv;
    // 短距离里程反码(用于校验)
    u32 subtotal_mileage_inv;
} save_info_t;

typedef struct
{
    u32 total_mileage;
    u32 subtotal_mileage;

    u32 engine_speed; // 发动机的转速（单位：rpm）

    u16 speed; // 时速(单位：km/h，使用英制单位时，需要进行转换)

    u8 fuel_percent; // 油量(单位：百分比)

    gear_t gear; // 档位

    u8 gear_valid;       // 档位是否有效
    u8 left_turn_valid;  // 左转灯是否有效
    u8 right_turn_valid; // 右转灯是否有效
    u8 low_beam_valid;   // 近光灯是否有效
    u8 high_beam_valid;  // 远光灯是否有效
    u8 bat_valid;        // 电池是否有效

    // 是否显示总里程(0：不显示ODO，显示TRIP，1：显示ODO，不显示TRIP)
    u8 is_display_total_mileage;

    // 提供给ui的数据
    u8 speed_to_display; // 显示屏显示的时速
} instrument_t;
extern volatile instrument_t instrument;

void instrument_info_init(void);
void instrument_info_save(void);

void instrument_info_save_time_add(void);
void instrument_info_save_enable(void);
void instrument_info_save_handle(void);

#endif
