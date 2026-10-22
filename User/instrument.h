#ifndef __INSTRUMENT_H__
#define __INSTRUMENT_H__

#include "typedef.h"

// 延时写入 EEPROM 的时间（单位：ms）：
// 由 2000 改成 100。车速较快时，两次保存之间累积的里程会更多
// （120km/h 时等 2s 就多走 67m），缩短延时才能把掉电时最多丢失的里程
// 压在“每 100m 保存一次”的量级上。
#define INSTRUMENT_INFO_SAVE_TIME ((u16)100)

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

/*
    EEPROM 里一条记录（一次保存）的布局，占 2 个页、共 16 字节，
    槽位划分见 eeprom_24cxx.h 的 EEPROM_SLOT_* 宏。

        数据页（第 0 页）：rec[0..3] 大计里程，rec[4..7] 小计里程（单位：m，小端）
        提交页（第 1 页）：CRC16 及反码、标志位及反码、固定标记；
                           提交页最后写，两页之间掉电时 CRC 对不上，
                           这一条记录会被跳过，上一条记录仍然有效。

    记录不再用结构体直接写入 EEPROM：结构体的字段顺序/对齐受编译器影响，
    而且 CRC 需要按字节计算，所以用字节缓冲打包。
*/
// 一条记录的大小，单位：字节
#define INSTRUMENT_EEPROM_REC_SIZE ((u8)16)

// 记录内各字段相对记录首地址的偏移
enum
{
    INSTRUMENT_EEPROM_OFS_TOTAL = 0,      // 4byte 大计里程（单位：m）
    INSTRUMENT_EEPROM_OFS_SUBTOTAL = 4,   // 4byte 小计里程（单位：m）
    INSTRUMENT_EEPROM_OFS_CRC = 8,        // 2byte CRC16（覆盖 0~7 和 12）
    INSTRUMENT_EEPROM_OFS_CRC_INV = 10,   // 2byte CRC16 的反码
    INSTRUMENT_EEPROM_OFS_FLAGS = 12,     // 1byte 标志位
    INSTRUMENT_EEPROM_OFS_FLAGS_INV = 13, // 1byte 标志位的反码
    INSTRUMENT_EEPROM_OFS_MAGIC = 14,     // 2byte 固定标记，确认提交页已写完
};

// 提交页上的固定标记
#define INSTRUMENT_EEPROM_MAGIC0 ((u8)0xA5)
#define INSTRUMENT_EEPROM_MAGIC1 ((u8)0x5A)

// 标志位的定义
#define INSTRUMENT_EEPROM_FLAG_DISPLAY_TOTAL ((u8)0x01) // bit0：1--显示总里程

// 里程的合理上限，超出则认为记录无效（空片读出来是全 0xFF）
#define INSTRUMENT_TOTAL_MILEAGE_MAX ((u32)999999 * 1000) // 999 999 km
#define INSTRUMENT_SUBTOTAL_MILEAGE_MAX ((u32)999999 * 100) // 99 999.9 km

typedef struct
{
    u32 total_mileage;    // 大计里程（单位：m），掉电时保存在 EEPROM 里
    u32 subtotal_mileage; // 小计里程（单位：m），长按清零，掉电时保存在 EEPROM 里

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
