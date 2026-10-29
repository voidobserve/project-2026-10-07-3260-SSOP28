#include <string.h> // memcpy(), memcmp()
#include "instrument.h"

#include "eeprom_24cxx.h"
#include "user_utils.h"
#include "user_config.h"

// 全局变量，存放仪表的相关数据
volatile instrument_t instrument = {0};

// 是否开始写入数据的时间计数
static volatile u8 instrument_info_save_time_add_enable = 0;
// 写入数据的时间计数
static volatile u16 instrument_info_save_time_cnt = 0;
// 写入时间到来，执行写入操作
static volatile u8 instrument_info_save_time_arrive = 0;

// 环形日志：下一次写入的槽位编号，上电时由 instrument_info_init() 从 EEPROM 里恢复
static u8 instrument_info_next_slot = 0;

// 上一次成功写入的槽位、以及当时写进去的里程：
// 里程没变、只有显示模式变了时，原地重写这一槽，避免日志里出现两条
// 里程相同、显示模式不同的记录（上电扫描时无法判断哪一条更新）
static u8 instrument_info_last_slot = 0;
static u32 instrument_info_last_total = 0;
static u32 instrument_info_last_subtotal = 0;

// 一条记录的打包缓冲（写入 EEPROM 用）
static u8 instrument_info_rec[INSTRUMENT_EEPROM_REC_SIZE];
// 回读校验缓冲（写完把记录读回来比较用）
static u8 instrument_info_chk[INSTRUMENT_EEPROM_REC_SIZE];

// CRC16 的算法参数（CRC-16/CCITT-FALSE）：多项式 0x1021，初值 0xFFFF
#define CRC16_CCITT_POLY    ((u16)0x1021)
#define CRC16_CCITT_INIT    ((u16)0xFFFF)
#define CRC16_MSB_MASK      ((u16)0x8000) // 判断当前最高位是否为 1
#define CRC16_BITS_PER_BYTE ((u8)8)       // 每个字节 8 bit

/**
 * @brief 计算 CRC16（CRC-16/CCITT-FALSE）
 * @note  只在保存和上电扫描时对 9 个字节计算，用按位算法即可，不占查表空间
 *
 * @param buf : 待计算的数据
 * @param len : 数据长度
 * @return 计算出来的 CRC16
 */
static u16 instrument_info_crc16(const u8 *buf, u16 len)
{
    u16 crc = CRC16_CCITT_INIT; // CRC 计算值
    u8 i;                       // 位计数
    u8 dat;                     // 当前处理的字节

    while (len != 0) {
        dat = *buf;
        buf++;
        len--;

        crc ^= (u16)dat << 8;
        for (i = 0; i < CRC16_BITS_PER_BYTE; i++) {
            if (0 != (crc & CRC16_MSB_MASK)) {
                crc = (u16)(crc << 1) ^ CRC16_CCITT_POLY;
            } else {
                crc = (u16)(crc << 1);
            }
        }
    }

    return crc;
}

/**
 * @brief 把 u32 按小端顺序打包到缓冲区里（不依赖编译器的结构体布局）
 *
 * @param p   : 目标缓冲区（至少 4 字节）
 * @param val : 待打包的值
 */
static void instrument_info_put_u32(u8 *p, u32 val)
{
    p[0] = (u8)val;
    p[1] = (u8)(val >> 8);
    p[2] = (u8)(val >> 16);
    p[3] = (u8)(val >> 24);
}

/**
 * @brief 从缓冲区里按小端顺序取出 u32
 *
 * @param p : 源缓冲区（至少 4 字节）
 * @return 取出来的值
 */
static u32 instrument_info_get_u32(const u8 *p)
{
    return ((u32)p[0]) | ((u32)p[1] << 8) | ((u32)p[2] << 16) |
           ((u32)p[3] << 24);
}

/**
 * @brief 校验一条记录，并取出里面的里程和标志位
 *
 * @param rec      : 记录缓冲区
 * @param total    : 取出的大计里程（单位：m）
 * @param subtotal : 取出的小计里程（单位：m）
 * @param flags    : 取出的标志位
 *
 * @retval 1--记录有效，0--记录无效（空片 / 掉电写坏 / 页间数据混合）
 */
static u8 instrument_info_record_parse(const u8 *rec, u32 *total, u32 *subtotal,
                                       u8 *flags)
{
    // CRC 覆盖“数据页 8 字节 + 标志位 1 字节”
    u8 crc_buf[EEPROM_PAGE_SIZE + 1];
    u16 crc;     // 记录里存放的 CRC16
    u16 crc_inv; // 记录里存放的 CRC16 反码

    crc = (u16)rec[INSTRUMENT_EEPROM_OFS_CRC];
    crc |= (u16)rec[INSTRUMENT_EEPROM_OFS_CRC + 1] << 8;
    crc_inv = (u16)rec[INSTRUMENT_EEPROM_OFS_CRC_INV];
    crc_inv |= (u16)rec[INSTRUMENT_EEPROM_OFS_CRC_INV + 1] << 8;

    // CRC 和它的反码必须互为反码，防止 CRC 本身被写坏
    if ((u16)~crc != crc_inv) {
        return 0;
    }

    // 重新计算 CRC，确认数据页和标志位没有被撕裂写破坏
    memcpy(crc_buf, rec, EEPROM_PAGE_SIZE);
    crc_buf[EEPROM_PAGE_SIZE] = rec[INSTRUMENT_EEPROM_OFS_FLAGS];
    if (instrument_info_crc16(crc_buf, EEPROM_PAGE_SIZE + 1) != crc) {
        return 0;
    }

    // 标志位与反码、提交页的固定标记
    if ((u8)~rec[INSTRUMENT_EEPROM_OFS_FLAGS] !=
        rec[INSTRUMENT_EEPROM_OFS_FLAGS_INV]) {
        return 0;
    }
    if ((INSTRUMENT_EEPROM_MAGIC0 != rec[INSTRUMENT_EEPROM_OFS_MAGIC]) ||
        (INSTRUMENT_EEPROM_MAGIC1 != rec[INSTRUMENT_EEPROM_OFS_MAGIC + 1])) {
        return 0;
    }

    *total = instrument_info_get_u32(&rec[INSTRUMENT_EEPROM_OFS_TOTAL]);
    *subtotal = instrument_info_get_u32(&rec[INSTRUMENT_EEPROM_OFS_SUBTOTAL]);
    *flags = rec[INSTRUMENT_EEPROM_OFS_FLAGS];

    // 合理范围检查：全 0xFF 的空片即使 CRC 巧合通过，也会被这里挡掉
    if ((*total > INSTRUMENT_TOTAL_MILEAGE_MAX) ||
        (*subtotal > INSTRUMENT_SUBTOTAL_MILEAGE_MAX)) {
        return 0;
    }

    return 1;
}

/**
 * @brief 仪表信息初始化：遍历整个 EEPROM，取出校验通过且最新的里程
 *
 * @note  调用时机在 tmr1_config() 之前（中断还没开），不存在并发问题
 */
void instrument_info_init(void)
{
    u8 slot;              // 当前扫描的槽位编号
    u8 found = 0;         // 是否找到过有效记录
    u8 read_err = 0;      // 扫描过程中是否出现过总线读失败
    u8 flags;             // 当前记录里的标志位
    u8 best_slot = 0;     // 有效记录中最新一条所在的槽位
    u8 best_flags = 0;    // 最新一条记录里的标志位
    u32 total;            // 当前记录里的大计里程（单位：m）
    u32 subtotal;         // 当前记录里的小计里程（单位：m）
    u32 max_total = 0;    // 目前找到的最大的大计里程（单位：m）
    u32 max_subtotal = 0; // 最新一条记录里的小计里程（单位：m）

    // 遍历 EEPROM ，找出校验通过、里程最大的那条记录
    for (slot = 0; slot < EEPROM_SLOT_NUMS; slot++) {
        if (0 != eeprom_24cxx_read(EEPROM_SLOT_TO_REG_ADDR(slot),
                                   instrument_info_rec,
                                   INSTRUMENT_EEPROM_REC_SIZE)) {
            read_err = 1; // 总线读失败，不能当成“没有数据”
            continue;
        }

        if (0 == instrument_info_record_parse(instrument_info_rec, &total,
                                              &subtotal, &flags)) {
            continue; // 校验不通过（空片 / 掉电写坏 / 页间数据混合），跳过
        }

        /*
            大计里程只会增加，所以大计里程最大的那条就是最新的一条；
            大计里程相同时，小计里程更小的一定是 TRIP 清零之后写的，取它。
        */
        if ((0 == found) || (total > max_total) ||
            ((total == max_total) && (subtotal < max_subtotal))) {
            found = 1;
            best_slot = slot;
            best_flags = flags;
            max_total = total;
            max_subtotal = subtotal;
        }
    }

    if (0 != found) {
        // 如果找到过有效记录，就取最新一条的里程和标志位
        instrument.total_mileage = max_total;
        instrument.subtotal_mileage = max_subtotal;
        if (0 != (best_flags & INSTRUMENT_EEPROM_FLAG_DISPLAY_TOTAL)) {
            instrument.is_display_total_mileage = 1;
        } else {
            instrument.is_display_total_mileage = 0;
        }

        // 下一次写入从最新记录的下一个槽位开始，写到末尾后回到开头
        instrument_info_next_slot = (u8)(best_slot + 1);
        if (instrument_info_next_slot >= EEPROM_SLOT_NUMS) {
            instrument_info_next_slot = 0;
        }

        // 记录下最新一条的内容，供“只有显示模式变化”时原地重写用
        instrument_info_last_slot = best_slot;
        instrument_info_last_total = max_total;
        instrument_info_last_subtotal = max_subtotal;
        return;
    }

    if (0 != read_err) {
        /*
            EEPROM 读失败：里面可能还有有效里程，保持 RAM 里的 0，且不要写，
            避免把原来的里程覆盖掉；下一次保存时会重新尝试。
        */
        instrument_info_next_slot = 0;
        return;
    }

    // 空片（第一次上电）：里程清零，并立刻写一条 (0,0)，让下次上电有基准数据
    instrument.total_mileage = 0;
    instrument.subtotal_mileage = 0;
    instrument.is_display_total_mileage = 1; // 默认显示总里程
    instrument_info_next_slot = 0;
    instrument_info_save();
}

/**
 * @brief 把当前里程写入 EEPROM
 *
 * @note  正常情况写环形日志的下一个槽位；里程没变、只有显示模式变了时，
 *        原地重写上一次的槽位，避免日志里出现两条里程相同、显示模式不同的记录。
 *        先写数据页、再写提交页：两页之间掉电时，提交页里的 CRC 还是上一次的，
 *        和新的数据页对不上，这一条记录会被跳过，上一条记录仍然完整。
 */
void instrument_info_save(void)
{
    // CRC 覆盖“数据页 8 字节 + 标志位 1 字节”
    u8 crc_buf[EEPROM_PAGE_SIZE + 1];
    u8 slot; // 本次写入的槽位编号
    u8 addr; // 本次写入的寄存器地址
    u16 crc; // 计算出来的 CRC16

    // 里程没变、只有显示模式变了：原地重写上一次的槽位
    if ((instrument.total_mileage == instrument_info_last_total) &&
        (instrument.subtotal_mileage == instrument_info_last_subtotal)) {
        slot = instrument_info_last_slot;
    } else {
        // 里程变了，写环形日志的下一个槽位
        slot = instrument_info_next_slot;
    }

    addr = EEPROM_SLOT_TO_REG_ADDR(slot);

    // 数据页：大计里程 + 小计里程
    instrument_info_put_u32(&instrument_info_rec[INSTRUMENT_EEPROM_OFS_TOTAL],
                            instrument.total_mileage);
    instrument_info_put_u32(
        &instrument_info_rec[INSTRUMENT_EEPROM_OFS_SUBTOTAL],
        instrument.subtotal_mileage);

    // 标志位(is_display_total_mileage，是否显示大计里程)
    instrument_info_rec[INSTRUMENT_EEPROM_OFS_FLAGS] =
        (0 != instrument.is_display_total_mileage)
            ? INSTRUMENT_EEPROM_FLAG_DISPLAY_TOTAL
            : 0;

    // 提交页：CRC16 覆盖“数据页 + 标志位”
    memcpy(crc_buf, instrument_info_rec, EEPROM_PAGE_SIZE);
    crc_buf[EEPROM_PAGE_SIZE] =
        instrument_info_rec[INSTRUMENT_EEPROM_OFS_FLAGS];
    crc = instrument_info_crc16(crc_buf, EEPROM_PAGE_SIZE + 1);

    instrument_info_rec[INSTRUMENT_EEPROM_OFS_CRC] = (u8)crc;
    instrument_info_rec[INSTRUMENT_EEPROM_OFS_CRC + 1] = (u8)(crc >> 8);
    instrument_info_rec[INSTRUMENT_EEPROM_OFS_CRC_INV] = (u8)~crc;
    instrument_info_rec[INSTRUMENT_EEPROM_OFS_CRC_INV + 1] = (u8)(~crc >> 8);
    instrument_info_rec[INSTRUMENT_EEPROM_OFS_FLAGS_INV] =
        (u8)~instrument_info_rec[INSTRUMENT_EEPROM_OFS_FLAGS];
    instrument_info_rec[INSTRUMENT_EEPROM_OFS_MAGIC] = INSTRUMENT_EEPROM_MAGIC0;
    instrument_info_rec[INSTRUMENT_EEPROM_OFS_MAGIC + 1] =
        INSTRUMENT_EEPROM_MAGIC1;

    // 写入失败时不推进槽位，下次保存重写这一槽
    if (0 != eeprom_24cxx_write(addr, instrument_info_rec, EEPROM_PAGE_SIZE)) {
        return;
    }
    if (0 != eeprom_24cxx_write((u8)(addr + EEPROM_PAGE_SIZE),
                                &instrument_info_rec[EEPROM_PAGE_SIZE],
                                EEPROM_PAGE_SIZE)) {
        return;
    }

    // 回读校验，确认数据真的写进去了
    if (0 != eeprom_24cxx_read(addr, instrument_info_chk,
                               INSTRUMENT_EEPROM_REC_SIZE)) {
        return;
    }
    if (0 != memcmp(instrument_info_rec, instrument_info_chk,
                    INSTRUMENT_EEPROM_REC_SIZE)) {
        return;
    }

    // 写入成功：如果写的是新槽位，就推进游标；写到末尾后回到开头继续
    if (slot == instrument_info_next_slot) {
        instrument_info_next_slot++;
        if (instrument_info_next_slot >= EEPROM_SLOT_NUMS) {
            instrument_info_next_slot = 0;
        }
    }

    // 记录本次写进去的内容，供下一次“原地重写”判断
    instrument_info_last_slot = slot;
    instrument_info_last_total = instrument.total_mileage;
    instrument_info_last_subtotal = instrument.subtotal_mileage;
}

// 使能延时写入 EEPROM 的操作
void instrument_info_save_enable(void)
{
    SYS_ENTER_CRITICAL_SECTION();
    instrument_info_save_time_arrive = 0;
    instrument_info_save_time_cnt = 0;
    instrument_info_save_time_add_enable = 1;
    SYS_EXIT_CRITICAL_SECTION();
}

/**
 * @brief 递增延时写入 EEPROM 的时间
 *
 * @note 在定时器中断内调用
 *
 * @return * void
 */
void instrument_info_save_time_add(void)
{
    if (instrument_info_save_time_add_enable) {
        if (instrument_info_save_time_cnt < UINT16_T_MAX) {
            instrument_info_save_time_cnt++;

            if (0 == instrument_info_save_time_arrive &&
                instrument_info_save_time_cnt >= INSTRUMENT_INFO_SAVE_TIME) {
                instrument_info_save_time_add_enable = 0; // 禁用计数
                instrument_info_save_time_cnt = 0;        // 重置计数
                instrument_info_save_time_arrive = 1;
            }
        }
    }
}

/**
 * @brief 检查是否可以保存数据到 EEPROM，如果可以，则保存数据
 *
 * @note “判断 + 清零”必须放在同一个临界区里：否则中断可能在判断之后、
 *       清零之前把标志重新置 1，这一次保存请求就被清掉了。
 */
void instrument_info_save_handle(void)
{
    u8 is_save_time_arrive; // 本次是否到了写入时间（从共享标志里取出的快照）

    SYS_ENTER_CRITICAL_SECTION();
    is_save_time_arrive = instrument_info_save_time_arrive;
    instrument_info_save_time_arrive = 0;
    SYS_EXIT_CRITICAL_SECTION();

    if (0 != is_save_time_arrive) {
        instrument_info_save();
    }
}
