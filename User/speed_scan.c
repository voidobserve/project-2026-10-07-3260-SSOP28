#include "speed_scan.h"
#include "instrument.h" // 包含仪表的参数
#include "mileage.h"    // 包含里程的参数 distance

#include "user_config.h"

// 速度检测是否一直没有脉冲到来，导致超时
// 等待主循环处理的超时事件
static volatile bit flag_is_speed_scan_over_time = 0;
// 超时已触发，等待检测到新脉冲后解除
static volatile bit speed_scan_timeout_latched = 0;
static volatile u32 speed_pulse_cnt = 0; // 当前窗口内累计的脉冲数
// 已完成但尚未处理的窗口脉冲总数
static volatile u32 speed_pending_pulse_cnt = 0;
static volatile u32 speed_pending_time_ms = 0; // 待处理窗口的累计时长，单位：ms
static volatile u16 speed_scan_time_ms = 0;    // 当前窗口已计时长度，单位：ms
static volatile u16 speed_no_pulse_time_ms = 0; // 距离上次脉冲的时间，单位：ms

// 滑动平局数组
#define SPEED_FILTER_ARRAY_SIZE (40)
// #define SPEED_FILTER_ARRAY_SIZE (20)
static volatile u8 speed_filter_array[SPEED_FILTER_ARRAY_SIZE] = {0};
static volatile u8 speed_filter_index = 0;

void speed_filter_init(u8 speed)
{
    u8 i;
    for (i = 0; i < SPEED_FILTER_ARRAY_SIZE; i++) {
        speed_filter_array[i] = speed;
    }

    speed_filter_index = 0;
}

void speed_filter_add(u8 speed)
{
    speed_filter_array[speed_filter_index] = speed;
    speed_filter_index++;
    if (speed_filter_index >= SPEED_FILTER_ARRAY_SIZE) {
        speed_filter_index = 0;
    }
}

u8 speed_filter_get_speed(void)
{
    u8 i;
    u32 sum = 0;
    u8 max_speed = 0;
    u8 min_speed = (u8)-1;

    // for (i = 0; i < SPEED_FILTER_ARRAY_SIZE; i++)
    // {
    //     printf("speed[%u] == %u\n", (u16)i, (u16)speed_filter_array[i]);
    // }
    // printf("\n===================\n");

    for (i = 0; i < SPEED_FILTER_ARRAY_SIZE; i++) {
        sum += speed_filter_array[i];

        if (max_speed < speed_filter_array[i]) {
            max_speed = speed_filter_array[i];
        }

        if (min_speed > speed_filter_array[i]) {
            min_speed = speed_filter_array[i];
        }
    }

    sum -= (max_speed + min_speed);

    return sum / (SPEED_FILTER_ARRAY_SIZE - 2);
}

// 时速扫描的配置
void speed_scan_config(void)
{
    P0_MD0 &= ~GPIO_P01_MODE_SEL(0x03); // 输入模式
}

void speed_scan_timer_50us_isr(void)
{
    // 记录上一次检测到的引脚电平（时速检测脚）
    static volatile bit last_speed_scan_level = 0;
    static volatile u16 cnt = 0; // 记录时速扫描的时间
    cnt++;
    if (cnt >= 20) // 每1ms进入一次
    {
        cnt = 0;
        speed_scan_time_ms++;
        if (speed_no_pulse_time_ms < SPEED_SCAN_OVER_TIME) {
            speed_no_pulse_time_ms++;
            if (speed_no_pulse_time_ms >= SPEED_SCAN_OVER_TIME &&
                speed_scan_timeout_latched == 0) {
                speed_scan_timeout_latched = 1;
                flag_is_speed_scan_over_time = 1;
            }
        }

        if (speed_scan_time_ms >= SPEED_SCAN_UPDATE_TIME) {
            speed_pending_pulse_cnt += speed_pulse_cnt;
            speed_pulse_cnt = 0;
            speed_pending_time_ms += SPEED_SCAN_UPDATE_TIME;
            speed_scan_time_ms = 0;
        }
    }

    if (SPEED_SCAN_PIN) {
        if (0 == last_speed_scan_level) {
            speed_pulse_cnt++;
            speed_no_pulse_time_ms = 0;
            speed_scan_timeout_latched = 0;
            flag_is_speed_scan_over_time = 0;
        }

        last_speed_scan_level = 1;
    } else {
        // 如果现在检测到低电平
        last_speed_scan_level = 0;
    }
}

void speed_scan(void)
{
    u32 cur_speed = 0;
    u32 tmp = 0;
    u32 speed_scan_time_snapshot = 0; // 本次消费的窗口总时长，单位：ms
    u32 speed_pulse_snapshot = 0;     // 本次消费的窗口脉冲总数
    u8 scan_due = 0;                  // 是否有完整窗口或待处理超时事件
    u8 is_speed_scan_over_time = 0;   // 本次快照是否包含超时事件

    ENTER_CRITICAL_SECTION();
    if (speed_pending_time_ms != 0 || flag_is_speed_scan_over_time) {
        // 有未处理完的窗口，或者时扫描超时
        speed_pulse_snapshot = speed_pending_pulse_cnt;
        speed_scan_time_snapshot = speed_pending_time_ms;
        is_speed_scan_over_time = flag_is_speed_scan_over_time;

        speed_pending_pulse_cnt = 0;
        speed_pending_time_ms = 0;
        flag_is_speed_scan_over_time = 0;
        scan_due = 1;
    }
    EXIT_CRITICAL_SECTION();

    if (0 == scan_due) {
        return;
    }

    /*
            采集到的脉冲个数 / 一圈对应的脉冲个数 * 车轮一圈对应走过的距离（单位：mm），
            计算得到 采集的脉冲个数对应走过的距离（单位：mm）
        */
    tmp = ((speed_pulse_snapshot * SPEED_SCAN_MM_PER_TURN) *
           SPEED_SCAN_PULSE_COMPENSATION / SPEED_SCAN_PULSE_PER_TURN);

    // printf("speed_pulse_snapshot %lu\n", speed_pulse_snapshot);

    if (is_speed_scan_over_time) {
        // 超时，采集到的脉冲个数对应一直是0km/h，认为时速是0
        // printf("scan over time\n");
        if (speed_pulse_snapshot != 0) {
            // 如果采集的脉冲个数不为0
            cur_speed = 1;
        } else {
            cur_speed = 0;
        }
    } else {
        // 未超时，计算采集到的脉冲个数对应走过的距离，再转换成以 km/h 的单位
        /*
                采集的脉冲个数对应走过的距离（单位：mm）/ 采集所用的时间（单位：ms） == 速度（单位：mm/ms）
                1mm/ms == 1m/s
                因为 1mm/ms * 1000 == 1m/ms
                     1m/ms  / 1000 == 1m/s
                     先乘以1000再除以1000，那么这个操作就可以去掉，直接化简为 1mm/ms == 1m/s

                1m/s == 3.6km/h，那么根据得到的 以 m/s 为单位的速度，先乘以36再除以10，得到以km/h为单位的速度值

                cur_speed == 采集的脉冲个数对应走过的距离（单位：mm）/ 采集所用的时间（单位：ms）* 3.6
                换成单片机可以计算的格式：
                cur_speed == 采集的脉冲个数对应走过的距离（单位：mm） * 36 / 10 / 采集所用的时间（单位：ms）
            */
        cur_speed = tmp * 36 / 10 / speed_scan_time_snapshot;
    }

    // 防止时速为0时（有可能是推车，记录不到速度），记录不到里程
    distance += tmp;

    // printf("cur distace 2 %lu\n", distance);
    // printf("cur distace %lu\n", distance);
    // printf("cur speed == %lu \n", cur_speed);

    if (cur_speed > 0) {
        // 显示时加入了一阶低通滤波，这里给得到的速度加上补偿
        cur_speed = (u32)cur_speed * 105 / 100;
    }

    // 限制要发送的时速:
    if (cur_speed > 199) {
        cur_speed = 199;
    }

    instrument.speed = cur_speed;
}
