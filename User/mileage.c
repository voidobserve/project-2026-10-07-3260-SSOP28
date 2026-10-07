// encoding UTF-8
#include "mileage.h"
#include "instrument.h"

// 里程扫描所需的计数值,每隔一定时间将里程写入flash
volatile u16 mileage_save_time_cnt;
// 存放每次扫描时走过的路程（单位：毫米）-->用于里程表的计数
volatile u32 distance;

// 里程更新的时间计数,每隔一段时间更新一次当前里程（负责控制发送里程的周期）
volatile u16 mileage_update_time_cnt;

// 总里程扫描
void mileage_scan(void)
{
    /*
        是否有里程数据需要保存的标志变量，
        0--没有里程变化，不需要保存，
        1--有里程变化，需要保存
        目前每过1m就会置位一次，保存之后清零
    */
    static volatile bit flag_is_any_mileage_save;

    // 每过1s，且里程有变化，就保存一次；这个里程变化的条件最好大于10m，否则会经常写入eeprom
    if ((mileage_save_time_cnt >= (u16)5 * 1000) && /* xx ms后 */
        flag_is_any_mileage_save)                   /* 里程有变化，需要保存 */
    {
        instrument_info_save_enable();
        flag_is_any_mileage_save = 0;
        mileage_save_time_cnt = 0;

        // printf("mile save\n");

        // printf("total_mileage %lu\n", instrument.save_info.total_mileage);
        // printf("sub_total_mileage %lu\n",
        // instrument.save_info.subtotal_mileage); printf("sub_total_mileage_2
        // %lu\n", instrument.save_info.subtotal_mileage_2);
    }

    if (distance >= 1000) // 1000mm -- 1m
    {
        // 如果走过的距离超过了1m，再进行保存（保存到变量）
        // 99 9999 KM
        if (instrument.save_info.total_mileage < (u32)(999999 * 1000)) {
            instrument.save_info.total_mileage++; // +1m
        }

        // 99999.9 KM
        if (instrument.save_info.subtotal_mileage < (u32)(999999 * 100)) {
            instrument.save_info.subtotal_mileage++; // +1m
        }

        distance -= 1000; // 剩下的、未保存的、不满1m的数据留到下一次再保存

        {
            static u8 cnt = 0;
            cnt++;
            // if (cnt >= 10) // cnt >= 10，说明走过了10m
            if (cnt >= 100) {
                cnt = 0;
                flag_is_any_mileage_save = 1; // 表示需要把里程输入写入到flash
            }
        }
    }
}
