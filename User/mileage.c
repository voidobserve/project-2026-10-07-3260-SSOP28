// encoding UTF-8
#include "mileage.h"
#include "instrument.h"

#include "user_utils.h"
#include "user_config.h"

// 存放每次扫描时走过的路程（单位：毫米）-->用于里程表的计数
volatile u32 distance;

// TMR1 中断累加的 1ms 计数（生产者），在 mileage_scan() 里被取走并清零（消费者）
volatile u16 mileage_time_1ms_cnt = 0;

// 距离上一次保存经过的时间（单位：ms），主循环自己维护，不会被中断修改
static u16 mileage_save_time_ms = 0;
// 车速为 0 持续的时间（单位：ms），主循环自己维护，不会被中断修改
static u16 mileage_stop_time_ms = 0;

/**
 * @brief 取走并清零中断里累加的 1ms 计数（生产者-消费者）
 *
 * @note  u16 变量在 8 位机上需要多次字节访问，“读取 + 清零”必须在同一个临界区里
 *        完成，否则读取过程中被 TMR1 中断打断，会读到撕裂的、偏小的值
 *
 * @return 本次取走的毫秒数
 */
static u16 mileage_time_1ms_take(void)
{
    u16 ms; // 本次取走的毫秒数

    SYS_ENTER_CRITICAL_SECTION();
    ms = mileage_time_1ms_cnt;
    mileage_time_1ms_cnt = 0;
    SYS_EXIT_CRITICAL_SECTION();

    return ms;
}

/**
 * @brief 累加计时，并饱和在 max，避免 u16 回绕后计时被清掉
 *
 * @param cnt : 计时变量
 * @param add : 本次要累加的毫秒数
 * @param max : 计时上限
 */
static void mileage_time_add(u16 *cnt, u16 add, u16 max)
{
    if (*cnt >= max) {
        return;
    }

    *cnt += add;
    if (*cnt > max) {
        *cnt = max;
    }
}

// 总里程扫描
void mileage_scan(void)
{
    // 走过 MILE_SAVE_DISTANCE 米，需要写 EEPROM 的标志
    static volatile bit flag_is_mileage_100m_arrive;
    // 有 1m 及以上的里程还没有写进 EEPROM（停车补写用）
    static volatile bit flag_is_mileage_dirty;

    // 走过 1m 的计数，累计到 MILE_SAVE_DISTANCE 就置位保存标志
    static u8 mile_distance_cnt = 0;
    u16 ms; // 本次从中断计数里取走的毫秒数

    // 取走中断累加的 1ms 计数，后面都用主循环自己的计时变量做判断
    ms = mileage_time_1ms_take();

    // 车速不为 0 时，停车计时清零；为 0 时累加停车时间
    if (0 != instrument.speed) {
        mileage_stop_time_ms = 0;
    } else {
        mileage_time_add(&mileage_stop_time_ms, ms, MILE_STOP_SAVE_TIME);
    }

    // 距离上一次保存的时间（只需要知道有没有超过最小间隔）
    mileage_time_add(&mileage_save_time_ms, ms, MILE_SAVE_MIN_INTERVAL);

    // 走过 MILE_SAVE_DISTANCE：立即让写入生效
    // （ MILE_SAVE_MIN_INTERVAL 的最小间隔只是防止里程异常跳变时写得太频繁）
    if ((0 != flag_is_mileage_100m_arrive) &&
        (mileage_save_time_ms >= MILE_SAVE_MIN_INTERVAL)) {
        flag_is_mileage_100m_arrive = 0;
        flag_is_mileage_dirty = 0;
        mileage_save_time_ms = 0;
        instrument_info_save_enable();
    }

    // 车速为 0 已持续 MILE_STOP_SAVE_TIME ，但还有没写进 EEPROM 的里程：补写一次
    if ((mileage_stop_time_ms >= MILE_STOP_SAVE_TIME) &&
        (0 != flag_is_mileage_dirty)) {
        flag_is_mileage_dirty = 0;
        mileage_stop_time_ms = 0; // 重新计时，避免停车期间反复写
        instrument_info_save_enable();
    }

    if (distance >= MILE_MM_PER_METER) {
        // 如果走过的距离超过了1m，再进行保存（保存到变量）
        if (instrument.total_mileage < INSTRUMENT_TOTAL_MILEAGE_MAX) {
            instrument.total_mileage++; // +1m
        }

        if (instrument.subtotal_mileage < INSTRUMENT_SUBTOTAL_MILEAGE_MAX) {
            instrument.subtotal_mileage++; // +1m
        }

        // 剩下的、未保存的、不满1m的数据留到下一次再保存
        distance -= MILE_MM_PER_METER;
        // 有 1m 及以上的里程还没写进 EEPROM
        flag_is_mileage_dirty = 1;

        mile_distance_cnt++;
        // mile_distance_cnt >= MILE_SAVE_DISTANCE ，说明走过了 MILE_SAVE_DISTANCE 米
        if (mile_distance_cnt >= MILE_SAVE_DISTANCE) {
            mile_distance_cnt = 0;
            flag_is_mileage_100m_arrive = 1; // 到点了，要把里程写进 EEPROM
        }
    }
}
