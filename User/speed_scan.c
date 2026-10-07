#include "speed_scan.h"
// #include "aip3368h_display.h"
#include "instrument.h" // 包含仪表的参数
#include "mileage.h"    // 包含里程的参数 distance

// 速度检测是否一直没有脉冲到来，导致超时
static volatile bit flag_is_speed_scan_over_time = 0;

static volatile u32 speed_pulse_cnt = 0;    // 记录脉冲个数，在定时器中断累加
static volatile u32 speed_scan_time_ms = 0; // 记录扫描时间，在定时器中断累加

static volatile u32 cur_speed_scan_time = 0; // 当前检测时速所用时间
// 当前检测时速所用时间内检测到的脉冲个数
static volatile u32 cur_speed_scan_pulse = 0;

// 计数器，计数满一段时间后，更新显示
static volatile u8 aip3368h_display_speed_refresh_time_cnt = 0;

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
    P1_MD1 &= ~GPIO_P15_MODE_SEL(0x3); // 输入模式
}

/**
 * @brief 更新检测时速的数据，由定时中断调用
 *
 */
void speed_scan_update_data(void) //
{
    cur_speed_scan_time += speed_scan_time_ms;
    speed_scan_time_ms = 0;
    cur_speed_scan_pulse += speed_pulse_cnt;
    speed_pulse_cnt = 0;
}

void speed_scan_timer_50us_isr(void)
{
    static volatile bit last_speed_scan_level =
        0;                       // 记录上一次检测到的引脚电平（时速检测脚）
    static volatile u16 cnt = 0; // 记录时速扫描的时间
    cnt++;
    if (cnt >= 20) // 每1ms进入一次
    {
        cnt = 0;
        // 更新扫描时间（如果一直没有脉冲到来给它清零，这里会一直累加）
        speed_scan_time_ms++;

        if (speed_scan_time_ms >= SPEED_SCAN_OVER_TIME &&
            flag_is_speed_scan_over_time == 0) {
            speed_scan_time_ms = 0;
            flag_is_speed_scan_over_time = 1; // 说明超时，脉冲计数一直没有加一
        }
    }

    if (SPEED_SCAN_PIN) // 检测时速的引脚
    {
        if (0 == last_speed_scan_level) {
            speed_pulse_cnt++;
            speed_scan_update_data();
        }

        last_speed_scan_level = 1;
    } else {
        // 如果现在检测到低电平
        last_speed_scan_level = 0;
    }
}

void speed_scan(void)
{
    volatile u32 cur_speed = 0;
    volatile u32 tmp = 0;

    if (cur_speed_scan_time >= SPEED_SCAN_UPDATE_TIME ||
        flag_is_speed_scan_over_time) {
        /*
            采集到的脉冲个数 / 一圈对应的脉冲个数 * 车轮一圈对应走过的距离（单位：mm），
            计算得到 采集的脉冲个数对应走过的距离（单位：mm）
        */
        tmp = ((cur_speed_scan_pulse * SPEED_SCAN_MM_PER_TURN) *
               SPEED_SCAN_PULSE_COMPENSATION / SPEED_SCAN_PULSE_PER_TURN);

        // printf("cur_speed_scan_pulse %lu\n", cur_speed_scan_pulse);

        if (flag_is_speed_scan_over_time) // 超时，采集到的脉冲个数对应一直是0km/h，认为时速是0
        {
            // printf("scan over time\n");
            if (cur_speed_scan_pulse != 0) {
                // 如果采集的脉冲个数不为0
                cur_speed = 1;
            } else {
                cur_speed = 0;
            }
        } else // 未超时，计算采集到的脉冲个数对应走过的距离，再转换成以 km/h 的单位
        {
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
            cur_speed = tmp * 36 / 10 / cur_speed_scan_time;
        }

        // 防止时速为0时（有可能是推车，记录不到速度），记录不到里程
        distance += tmp;

        // printf("cur distace 2 %lu\n", distance);
        // printf("cur distace %lu\n", distance);
        // printf("cur speed == %lu \n", cur_speed);
        // printf("cur_speed_scan_pulse == %lu\n", cur_speed_scan_pulse);

        cur_speed_scan_pulse = 0;
        cur_speed_scan_time = 0;
        flag_is_speed_scan_over_time = 0;

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
}
