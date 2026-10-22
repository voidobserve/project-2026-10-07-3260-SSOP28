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
    环形日志的槽位划分（一条记录里各字段的含义见 instrument.h）：
    一次保存写一条记录，一条记录占 EEPROM_SLOT_PAGES 个页，正好 2 页 16 字节：
        第 0 页：4byte 大计里程 + 4byte 小计里程（单位：m）
        第 1 页：CRC16 + 反码 + 标志位 + 反码 + 固定标记（提交页，最后写）
    整片 32 页 / 2 页 = 16 个槽，写入时轮流使用：
    掉电最多损坏正在写的那一条，上一条记录始终完整。
*/
// 一条记录占用的页数
#define EEPROM_SLOT_PAGES ((u8)2)
// 一条记录的大小，单位：字节
#define EEPROM_SLOT_SIZE ((u8)(EEPROM_SLOT_PAGES * EEPROM_PAGE_SIZE))
// 槽位数量（整片 EEPROM 能存多少条记录）
#define EEPROM_SLOT_NUMS ((u8)(EEPROM_PAGE_NUMS / EEPROM_SLOT_PAGES))
// 将槽位编号转换成对应的寄存器地址
#define EEPROM_SLOT_TO_REG_ADDR(slot) ((u8)((u16)(slot) * EEPROM_SLOT_SIZE))

void eeprom_24cxx_config(void);

/**
 * @brief  向 24Cxx 写入一段数据，写完等待器件内部写周期结束
 * @note   24Cxx 单次写入不能跨页（页大小 EEPROM_PAGE_SIZE 字节），
 *         跨页时芯片的地址会在页内回卷，覆盖本页前面的数据，
 *         所以这里限制了“长度不超过一页、且不跨页”，跨页请分多次调用
 * @param  reg_addr : 存储单元地址
 * @param  buf      : 待写入数据的缓冲区首地址
 * @param  len      : 要写入的字节数（1 ~ EEPROM_PAGE_SIZE）
 * @retval 0--成功，非0--失败（1:入参非法 2:跨页 3:写失败 4:等待写周期超时）
 */
u8 eeprom_24cxx_write(u8 reg_addr, const u8 *buf, u16 len);

/**
 * @brief 从 24Cxx 读取一段数据
 * @note  读操作允许跨页：24Cxx 顺序读时地址会自动递增，读到末尾回到开头
 * @retval 0--成功，非0--失败
 */
u8 eeprom_24cxx_read(u8 reg_addr, u8 *buf, u16 len);

#if EEPROM_24CXX_TEST_ENABLE
extern volatile u8 flag_is_test_eeprom_24cxx_period_coming;
void test_eeprom_24cxx_period_1ms_isr(void);
void test_eeprom_24cxx(void);
void test_eeprom_24cxx_addr(void);
#endif

#endif
