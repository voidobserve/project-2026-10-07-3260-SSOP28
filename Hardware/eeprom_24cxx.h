#ifndef __EEPROM_24CXX_H__
#define __EEPROM_24CXX_H__

#include "typedef.h"

#define EEPROM_24CXX_TEST_ENABLE 0

/*
	器件地址，不包含读写操作位
*/
#define EEPROM_DEV_ADDR ((u8)0xA0 >> 1)

// 写完等待器件内部写周期(tWR)结束的超时时间(单位: ms)
// 24C02 的 tWR 最长 5ms，这里给 5ms + 5ms 余量
#define EEPROM_WAIT_READY_TIMEOUT_MS ((u16)(5 + 5))

// 程序限制的，每组页面最大的擦写次数
#define EEPROM_MAX_ERASE_COUNTS_PER_PAGE ((u32)80 * 10000)
// TEST ONLY 程序限制的，每组页面最大的擦写次数 -- 测试用（最大写到 n 次，包括第 n 次）
// #define EEPROM_MAX_ERASE_COUNTS_PER_PAGE ((u32)10)
#define EEPROM_PAGE_NUMS ((u16)32) // eeprom 页数量
#define EEPROM_PAGE_SIZE ((u8)(8)) // eeprom 页地址大小，单位：字节
// 将页面id转换成对应的寄存器地址
#define EEPROM_PAGE_TO_REG_ADDR(x) ((u16)(0x00 + (u16)(x) * EEPROM_PAGE_SIZE))

/*
    eeprom，用户划分的块大小，单位：页
    目前一个块存放的数据：
    4byte 大计里程，4byte 大计里程校验
    4byte 小计里程，4byte 小计里程校验
    一共 16 byte，2 页

    再加上双缓冲作为备份，
    总共 32 byte， 4 页
*/
#define EEPROM_USER_BLOCK_SIZE (4)

#if 0
// 用户设计的使用年限，单位：年
#define EEPROM_USER_DESIGN_USE_YEAR (100)
/*
    eeprom，用户划分的存储块数量
    == eeprom空间(单位：字节)  / 用户划分的块大小(单位：字节)
*/
#define EEPROM_BLOCK_NUMS           (EEPROM_PAGE_NUMS / EEPROM_USER_BLOCK_SIZE)

/*
    限制的，每次给eeprom写入数据的时间，单位：s
*/
#define EEPROM_WRITE_DATA_LIMIT_PERIOD                                         \
    (u32) EEPROM_MAX_ERASE_COUNTS_PER_PAGE * EEPROM_BLOCK_NUMS *               \
        EEPROM_USER_DESIGN_USE_YEAR / (60 * 60 * 24 * 365)

// TEST ONLY
enum
{
    val = EEPROM_WRITE_DATA_LIMIT_PERIOD,
};
#endif

// 数据页结构
typedef struct
{
    // 总里程表（单位：m）
    u32 total_mileage;
    // 短距离里程表(单位：m)
    u32 subtotal_mileage;
} eeprom_saveinfo_t;

void eeprom_24cxx_config(void);

void eeprom_24cxx_write(u8 reg_addr, u8 *buf, u16 len);
void eeprom_24cxx_read(u8 reg_addr, u8 *buf, u16 len);

#if EEPROM_24CXX_TEST_ENABLE
extern volatile u8 flag_is_test_eeprom_24cxx_period_coming;
void test_eeprom_24cxx_period_1ms_isr(void);
void test_eeprom_24cxx(void);
void test_eeprom_24cxx_addr(void);
#endif

#endif
