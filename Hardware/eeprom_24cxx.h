#ifndef __EEPROM_24CXX_H__
#define __EEPROM_24CXX_H__

#include "typedef.h"

/*
	器件地址，不包含读写操作位
*/
#define EEPROM_DEV_ADDR ((u8)0xA0 >> 1)
// 程序限制的，每组页面最大的擦写次数
#define EEPROM_MAX_ERASE_COUNTS_PER_PAGE ((u32)80 * 10000)
// TEST ONLY 程序限制的，每组页面最大的擦写次数 -- 测试用（最大写到 n 次，包括第 n 次）
// #define EEPROM_MAX_ERASE_COUNTS_PER_PAGE ((u32)10)
#define EEPROM_PAGE_NUMS ((u8)256) // eeprom 页数量
#define EEPROM_PAGE_SIZE ((u8)(8)) // eeprom 页地址大小，单位：字节
// 将页面id转换成对应的地址
#define EEPROM_PAGE_X_ADDR(x) ((u16)(0x00 + (u16)(x) * EEPROM_PAGE_SIZE))

typedef struct
{
    // 总里程表（单位：m，使用英制单位时，只需要再发送时进行转换）
    // （大计里程，范围：0~ 99999.9 KM）
    u32 total_mileage;
    // 短距离里程表(单位：m，使用英制单位时，只需要再发送时进行转换)
    // （小计里程，范围：0~9999.9 KM）
    u32 subtotal_mileage;

    u32 erase_cnt; // 擦写次数

    u8 is_data_valid;
} eeprom_saveinfo_t;

typedef struct
{
    u8 cur_write_page_id; // 当前写入的扇区ID （只在扇区目录中写入/读取时使用）
    u8 is_data_valid; // DATA_VALID_ID--数据有效，其他--数据无效（只在扇区目录中写入/读取时使用）
} eeprom_menu_t;

#endif
