#include "aip3368h_display.h"
#include "aip3368_driver.h"
#include "user_utils.h"

/**
 * @brief 7段数码管段码定义 (a,b,c,d,e,f,g)
 *        对应二进制位: bit0=a, bit1=b, bit2=c, bit3=d, bit4=e, bit5=f, bit6=g
 */
static const u8 digit_segment_code[10] = {
    0x3F, // 0: abcdef
    0x06, // 1: bc
    0x5B, // 2: abdeg
    0x4F, // 3: abcdg
    0x66, // 4: bcfg
    0x6D, // 5: acdfg
    0x7D, // 6: acdefg
    0x07, // 7: abc
    0x7F, // 8: abcdefg
    0x6F  // 9: abcdfg
};

/*
    指示灯和显存的映射关系
    小时个位对应的指示灯
    对应的显存：  
*/
static const aip3368h_display_mapping_t hour_bit_0_map[] = {
    // a 段 ~ g 段
    {8, 9}, {8, 10}, {8, 11}, {8, 12}, {8, 13}, {8, 14}, {8, 15},
};

/*
    指示灯和显存的映射关系
    小时十位对应的指示灯
    对应的显存：  
*/
static const aip3368h_display_mapping_t hour_bit_1_map[] = {
    // a 段 ~ g 段
    {9, 9}, {9, 10}, {9, 11}, {9, 12}, {9, 13}, {9, 14}, {9, 15},
};

/*
    指示灯和显存的映射关系
    分钟个位对应的指示灯
    对应的显存：  
*/
static const aip3368h_display_mapping_t minute_bit_0_map[] = {
    // a 段 ~ g 段
    {7, 0}, {7, 1}, {7, 2}, {7, 3}, {7, 4}, {7, 5}, {7, 6},
};

/*
    指示灯和显存的映射关系
    分钟十位对应的指示灯
    对应的显存：  
*/
static const aip3368h_display_mapping_t minute_bit_1_map[] = {
    // a 段 ~ g 段
    {8, 0}, {8, 1}, {8, 2}, {8, 3}, {8, 4}, {8, 5}, {8, 6},
};

/*
    指示灯和显存的映射关系
    里程对应的指示灯
    对应的显存：  

    [0][0] ~ [0][6]，里程第 0 位数码管的a段~g段（位从右往左，从0开始）
    [1][0] ~ [1][6]，里程第 1 位数码管的a段~g段（位从右往左，从0开始）
*/
static const aip3368h_display_mapping_t mileage_map[][7] = {
    {
        {11, 0},
        {11, 1},
        {11, 2},
        {11, 3},
        {11, 4},
        {11, 5},
        {11, 6},
    },

    {
        {11, 9},
        {11, 10},
        {11, 11},
        {11, 12},
        {11, 13},
        {11, 14},
        {11, 15},
    },

    {
        {10, 0},
        {10, 1},
        {10, 2},
        {10, 3},
        {10, 4},
        {10, 5},
        {10, 6},
    },

    {
        {10, 9},
        {10, 10},
        {10, 11},
        {10, 12},
        {10, 13},
        {10, 14},
        {10, 15},
    },

    {
        {12, 9},
        {12, 10},
        {12, 11},
        {12, 12},
        {12, 13},
        {12, 14},
        {12, 15},
    },
};

/*
    指示灯和显存的映射关系
    油量格数对应的指示灯
    对应的显存：  
*/
static const aip3368h_display_mapping_t fuel_lev_map[] = {
    {12, 1}, {12, 2}, {12, 3}, {12, 4}, {12, 5}, {12, 6},
};

/*
    指示灯和显存的映射关系
    发动机转速边框对应的指示灯
    对应的显存：  
*/
static const aip3368h_display_mapping_t engine_speed_frame_map[] = {
    {3, 0},  {3, 1},  {3, 2}, {3, 3},  {3, 4},  {3, 5},  {3, 6},
    {3, 7},  {3, 8},  {3, 9}, {3, 10}, {3, 11}, {3, 12}, {3, 13},
    {3, 14}, {3, 15}, {0, 8}, {0, 9},  {0, 10}, {0, 11}, {0, 12},
};

/*
    指示灯和显存的映射关系
    发动机转速对应的格数，从低到高
    对应的显存：  
*/
static const aip3368h_display_mapping_t engine_speed_lev_map[] = {
    {1, 0}, {1, 1}, {1, 2},  {1, 3},  {1, 4},  {1, 5},  {1, 6},  {1, 7},
    {1, 8}, {1, 9}, {1, 10}, {1, 11}, {1, 12}, {1, 13}, {1, 14}, {1, 15},
    {0, 0}, {0, 1}, {0, 2},  {0, 3},  {0, 4},  {0, 5},  {0, 6},  {0, 7},
};

void aip3368h_display_left_turn_light(u8 is_display)
{
    if (is_display) {
        AIP3368H_DISPLAY_BUF_SET(9, 7);
    } else {
        AIP3368H_DISPLAY_BUF_CLR(9, 7);
    }
}

void aip3368h_display_right_turn_light(u8 is_display)
{
    if (is_display) {
        AIP3368H_DISPLAY_BUF_SET(5, 0);
    } else {
        AIP3368H_DISPLAY_BUF_CLR(5, 0);
    }
}

void aip3368h_display_high_beam_light(u8 is_display)
{
    if (is_display) {
        AIP3368H_DISPLAY_BUF_SET(5, 1);
    } else {
        AIP3368H_DISPLAY_BUF_CLR(5, 1);
    }
}

void aip3368h_display_low_beam_light(u8 is_display)
{
    if (is_display) {
        AIP3368H_DISPLAY_BUF_SET(9, 8);
    } else {
        AIP3368H_DISPLAY_BUF_CLR(9, 8);
    }
}

/**
 * @brief 时钟图标对应的指示灯
 * 
 * @param is_display 
 */
void aip3368h_display_clock_light(u8 is_display)
{
    if (is_display) {
        AIP3368H_DISPLAY_BUF_SET(9, 6);
    } else {
        AIP3368H_DISPLAY_BUF_CLR(9, 6);
    }
}

/**
 * @brief 显示小时
 *      24小时制，小于10小时，不显示十位数，大于等于10小时，显示十位数
 * 
 * @param is_display
 * 
 * @param hour 
 */
void aip3368h_display_time_hour(u8 is_display, u8 hour)
{
    u8 i;
    u8 segment_code; // 段码

    if (is_display) {
        // 得到小时个位对应的段码
        segment_code = digit_segment_code[hour % 10];
        // 遍历 a ~ g 段数码管，设置显示
        for (i = 0; i < 7; i++) {
            // 检查该段是否需要点亮 ( segment_code 的对应 bit 是否为1)
            if (segment_code & (0x01 << i)) {
                AIP3368H_DISPLAY_BUF_SET(hour_bit_0_map[i].buff_index,
                                         hour_bit_0_map[i].bit_offset);
            } else {
                AIP3368H_DISPLAY_BUF_CLR(hour_bit_0_map[i].buff_index,
                                         hour_bit_0_map[i].bit_offset);
            }
        }

        if (hour >= 10) {
            // 得到小时十位对应的段码
            segment_code = digit_segment_code[hour / 10];
            // 遍历 a ~ g 段数码管，设置显示
            for (i = 0; i < 7; i++) {
                // 检查该段是否需要点亮 ( segment_code 的对应 bit 是否为1)
                if (segment_code & (0x01 << i)) {
                    AIP3368H_DISPLAY_BUF_SET(hour_bit_1_map[i].buff_index,
                                             hour_bit_1_map[i].bit_offset);
                } else {
                    AIP3368H_DISPLAY_BUF_CLR(hour_bit_1_map[i].buff_index,
                                             hour_bit_1_map[i].bit_offset);
                }
            }
        } else {
            // hour < 10， 清空小时的十位显示
            // 遍历 a ~ g 段数码管，清空显示
            for (i = 0; i < 7; i++) {
                AIP3368H_DISPLAY_BUF_CLR(hour_bit_1_map[i].buff_index,
                                         hour_bit_1_map[i].bit_offset);
            }
        }
    } else {
        // 遍历 a ~ g 段数码管，清空显示
        for (i = 0; i < 7; i++) {
            AIP3368H_DISPLAY_BUF_CLR(hour_bit_0_map[i].buff_index,
                                     hour_bit_0_map[i].bit_offset);
            AIP3368H_DISPLAY_BUF_CLR(hour_bit_1_map[i].buff_index,
                                     hour_bit_1_map[i].bit_offset);
        }
    }
}

/**
 * @brief 显示分钟
 * 
 * @param is_display 
 * @param minute 
 */
void aip3368h_display_time_minute(u8 is_display, u8 minute)
{
    u8 i;
    u8 segment_code; // 段码
    if (is_display) {
        segment_code = digit_segment_code[minute % 10];
        // 遍历 a ~ g 段数码管，设置显示
        for (i = 0; i < 7; i++) {
            // 检查该段是否需要点亮 ( segment_code 的对应 bit 是否为1)
            if (segment_code & (0x01 << i)) {
                AIP3368H_DISPLAY_BUF_SET(minute_bit_0_map[i].buff_index,
                                         minute_bit_0_map[i].bit_offset);
            } else {
                AIP3368H_DISPLAY_BUF_CLR(minute_bit_0_map[i].buff_index,
                                         minute_bit_0_map[i].bit_offset);
            }
        }

        segment_code = digit_segment_code[minute / 10];
        // 遍历 a ~ g 段数码管，设置显示
        for (i = 0; i < 7; i++) {
            // 检查该段是否需要点亮 ( segment_code 的对应 bit 是否为1)
            if (segment_code & (0x01 << i)) {
                AIP3368H_DISPLAY_BUF_SET(minute_bit_1_map[i].buff_index,
                                         minute_bit_1_map[i].bit_offset);
            } else {
                AIP3368H_DISPLAY_BUF_CLR(minute_bit_1_map[i].buff_index,
                                         minute_bit_1_map[i].bit_offset);
            }
        }

    } else {
        // 遍历 a ~ g 段数码管，清空显示
        for (i = 0; i < 7; i++) {
            AIP3368H_DISPLAY_BUF_CLR(minute_bit_0_map[i].buff_index,
                                     minute_bit_0_map[i].bit_offset);
            AIP3368H_DISPLAY_BUF_CLR(minute_bit_1_map[i].buff_index,
                                     minute_bit_1_map[i].bit_offset);
        }
    }
}

/**
 * @brief 时间中间的冒号对应的指示灯（时间分隔符）
 *
 * @param is_display 是否显示
 *
 */
void aip3368h_display_time_colon_light(u8 is_display)
{
    if (is_display) {
        AIP3368H_DISPLAY_BUF_SET(8, 7);
    } else {
        AIP3368H_DISPLAY_BUF_CLR(8, 7);
    }
}

/**
 * @brief 里程的小数点
 *
 * @param is_display
 */
void aip3368h_display_mileage_point_light(u8 is_display)
{
    if (is_display) {
        AIP3368H_DISPLAY_BUF_SET(11, 8);
    } else {
        AIP3368H_DISPLAY_BUF_CLR(11, 8);
    }
}

/**
 * @brief 里程 TRIP 字样对应的指示灯
 * 
 * @param is_display 
 */
void aip3368h_display_trip_light(u8 is_display)
{
    if (is_display) {
        AIP3368H_DISPLAY_BUF_SET(10, 7);
    } else {
        AIP3368H_DISPLAY_BUF_CLR(10, 7);
    }
}

/**
 * @brief 里程 ODO 字样对应的指示灯
 * 
 */
void aip3368h_display_odo_light(u8 is_display)
{
    if (is_display) {
        AIP3368H_DISPLAY_BUF_SET(10, 8);
    } else {
        AIP3368H_DISPLAY_BUF_CLR(10, 8);
    }
}

/**
 * @brief 里程 km 字样对应的指示灯
 * 
 * @param is_display 
 */
void aip3368h_display_km_light(u8 is_display)
{
    if (is_display) {
        AIP3368H_DISPLAY_BUF_SET(9, 0);
    } else {
        AIP3368H_DISPLAY_BUF_CLR(9, 0);
    }
}

/**
 * @brief 里程 mile 字样对应的指示灯
 * 
 * @param is_display 
 */
void aip3368h_display_mile_light(u8 is_display)
{
    if (is_display) {
        AIP3368H_DISPLAY_BUF_SET(11, 7);
    } else {
        AIP3368H_DISPLAY_BUF_CLR(11, 7);
    }
}

/**
 * @brief 里程对应的某一位数码管，显示对应数字或不显示
 *
 * @param is_display 1：显示，0：不显示
 *          如果为0，参数列表后续的参数无效
 * @param bit_x 数码管的位置，从右往左排序，从 0 开始
 * @param num
 * 
 */
void aip3368h_display_mileage_bit_x(u8 is_display, u8 bit_x, u8 num)
{
    u8 i;
    u8 segment_code = 0;
    segment_code = digit_segment_code[num];
    // 遍历数码管的 a ~ g 段
    for (i = 0; i < 7; i++) {
        if (is_display && (segment_code & (0x01 << i))) {
            // 如果当前位对应的数码管要显示，并且当前段对应的指示灯要点亮
            AIP3368H_DISPLAY_BUF_SET(mileage_map[bit_x][i].buff_index,
                                     mileage_map[bit_x][i].bit_offset);
        } else {
            AIP3368H_DISPLAY_BUF_CLR(mileage_map[bit_x][i].buff_index,
                                     mileage_map[bit_x][i].bit_offset);
        }
    }
}

/**
 * @brief 显示里程（不包括小数点、里程单位）
 * 
 * @param mileage 里程，只按有效数字进行显示 
 *      1： 显示 0.1
 *      10： 显示 1.0
 *      200： 显示 20.0
 * 
 */
void aip3368h_display_mileage(u32 mileage)
{
    u8 i;
    u8 valid_bits = 0; // 存放有效数据位
    u32 tmp = 0;       // 计算有效数据位使用的临时变量

    // 计算有效数字位
    tmp = mileage;
    while (1) {
        valid_bits++; // 如果是刚进入，默认至少有1位有效数据
        tmp /= 10;
        if (tmp == 0) {
            break;
        }
    }

    // 如果有效数字位大于5，则只显示第 0 ~ 4 位
    if (valid_bits > 5) {
        valid_bits = 5;
    } else if (valid_bits == 1) {
        // 如果有效数字位只有1位，需要显示 0.x 这样的效果，让 valid_bits 加一
        valid_bits = 2;
    }

    // 遍历里程的第 0 ~ valid_bits 位数码管(数码管从右往左排序)
    for (i = 0; i < 5; i++) {
        // i 从 0 开始，按位数直接判断即可；未显示位要清空对应的数码管
        if (i < valid_bits) {
            aip3368h_display_mileage_bit_x(1, i, (mileage % 10));
        } else {
            aip3368h_display_mileage_bit_x(0, i, 0);
        }

        mileage /= 10;
    }
}

/**
 * @brief 油量 F 字样(full)对应的指示灯
 * 
 */
void aip3368h_display_fuel_full_light(u8 is_display)
{
    if (is_display) {
        AIP3368H_DISPLAY_BUF_SET(12, 8);
    } else {
        AIP3368H_DISPLAY_BUF_CLR(12, 8);
    }
}

/**
 * @brief 油量 E 字样(empty)对应的指示灯
 * 
 */
void aip3368h_display_fuel_empty_light(u8 is_display)
{
    if (is_display) {
        AIP3368H_DISPLAY_BUF_SET(9, 1);
    } else {
        AIP3368H_DISPLAY_BUF_CLR(9, 1);
    }
}

/**
 * @brief 油量图标对应的指示灯
 * 
 */
void aip3368h_display_fuel_icon_light(u8 is_display)
{
    if (is_display) {
        AIP3368H_DISPLAY_BUF_SET(12, 7);
    } else {
        AIP3368H_DISPLAY_BUF_CLR(12, 7);
    }
}

/**
 * @brief 油量第 0 格，红色指示灯
 * 
 */
void aip3368h_display_fuel_lev_0_red_light(u8 is_display)
{
    if (is_display) {
        AIP3368H_DISPLAY_BUF_SET(12, 0);
    } else {
        AIP3368H_DISPLAY_BUF_CLR(12, 0);
    }
}

/**
 * @brief 油量格子数指示灯(不包括第 0 格红色指示灯)
 * 
 * @param is_display 
 * @param fuel_lev 1 ~ 6
 *      0：不显示
 *      1： 显示 1 格
 *      2： 显示 2 格   
 *      ...
 */
void aip3368h_display_fuel_lev(u8 is_display, u8 fuel_lev)
{
    u8 i;

    for (i = 0; i < ARRAY_SIZE(fuel_lev_map); i++) {
        if (is_display && (i < fuel_lev)) {
            AIP3368H_DISPLAY_BUF_SET(fuel_lev_map[i].buff_index,
                                     fuel_lev_map[i].bit_offset);
        } else {
            AIP3368H_DISPLAY_BUF_CLR(fuel_lev_map[i].buff_index,
                                     fuel_lev_map[i].bit_offset);
        }
    }
}

/**
 * @brief 根据索引，点亮发动机转速边框对应的指示灯
 * 
 */
void aip3368h_display_engine_speed_frame_by_idx(u8 is_display, u8 idx)
{
    if (is_display) {
        AIP3368H_DISPLAY_BUF_SET(engine_speed_frame_map[idx].buff_index,
                                 engine_speed_frame_map[idx].bit_offset);
    } else {
        AIP3368H_DISPLAY_BUF_CLR(engine_speed_frame_map[idx].buff_index,
                                 engine_speed_frame_map[idx].bit_offset);
    }
}

/**
 * @brief 显示发动机转速
 * 
 * @param is_display 
 *          1：显示
 *          0：不显示，参数列表后续的参数无效
 * @param engine_speed_lev 发动机转速等级  
 *          0：不显示
 *          1：显示 1 格
 *          2：显示 2 格
 *          ...
 * 
 */
void aip3368h_display_engine_speed_lev(u8 is_display, u16 engine_speed_lev)
{
    u8 i;

    for (i = 0; i < ARRAY_SIZE(engine_speed_lev_map); i++) {
        if (is_display && (i < engine_speed_lev)) {
            AIP3368H_DISPLAY_BUF_SET(engine_speed_lev_map[i].buff_index,
                                     engine_speed_lev_map[i].bit_offset);
        } else {
            AIP3368H_DISPLAY_BUF_CLR(engine_speed_lev_map[i].buff_index,
                                     engine_speed_lev_map[i].bit_offset);
        }
    }
}

#if AIP3368H_DISPLAY_TEST_ENABLE
/**
 * @brief 测试指示灯
 * 
 * @return * void 
 */
void test_aip3368h_display_light_1ms_isr(void)
{
    static u16 cnt = 0; // 计数，用于控制显示的频率
    static u8 is_display = 0;
    cnt++;
    if (cnt < 500) {
        return;
    } else {
        cnt = 0;
    }

    // aip3368h_display_left_turn_light(is_display);
    // aip3368h_display_right_turn_light(is_display);
    // aip3368h_display_high_beam_light(is_display);
    // aip3368h_display_low_beam_light(is_display);
    // aip3368h_display_clock_light(is_display);
    // aip3368h_display_time_colon_light(is_display);
    // aip3368h_display_time_minute(is_display, 95);
    // aip3368h_display_mileage_point_light(is_display);
    // aip3368h_display_trip_light(is_display);
    // aip3368h_display_odo_light(is_display);
    // aip3368h_display_km_light(is_display);
    // aip3368h_display_mile_light(is_display);
    // aip3368h_display_fuel_full_light(is_display);
    // aip3368h_display_fuel_empty_light(is_display);
    // aip3368h_display_fuel_icon_light(is_display);
    // aip3368h_display_fuel_lev_0_red_light(is_display);

    is_display = !is_display;
}

void test_aip3368h_display_engine_speed_lev_1ms_isr(void)
{
    static u16 cnt = 0; // 计数，用于控制显示的频率
    static u8 lev = 0;
    cnt++;
    if (cnt < 500) {
        return;
    } else {
        cnt = 0;
    }

    aip3368h_display_engine_speed_lev(1, lev);
    lev++;
    if (lev >= ARRAY_SIZE(engine_speed_lev_map) + 1) {
        lev = 0;
    }
}

#if 0

void test_aip3368h_display_engine_speed_frame_1ms_isr(void)
{
    static u16 cnt = 0; // 计数，用于控制显示的频率
    static u8 idx = 0;
    static u8 dir = 0;
    cnt++;
    if (cnt < 500) {
        return;
    } else {
        cnt = 0;
    }

    aip3368h_display_engine_speed_frame_by_idx(!dir, idx);
    if (0 == dir) {
        idx++;
        if (idx >= ARRAY_SIZE(engine_speed_frame_map)) {
            idx = ARRAY_SIZE(engine_speed_frame_map) - 1;
            dir = 1;
        }
    } else {
        idx--;
        // 如果刚好递减到溢出
        if ((u8)-1 == idx) {
            idx = 0;
            dir = 0;
        }
    }
}

/**
 * @brief 发动机转速边框动画：中间三盏灯向两侧扩散，再往中间回收，循环播放
 *
 *  动画过程：
 *  1. 从中间三个灯开始；
 *  2. 两个灯往左侧扩散，两个灯往右侧扩散；
 *  3. 到达边界后折返，向中间回收；
 *  4. 回到中间时，四个移动的灯会重合成中间三个灯，循环。
 */
void test_aip3368h_display_engine_speed_frame_wave_1ms_isr(void)
{
    static u16 cnt = 0; // 计数，用于控制显示的频率
    static s8 dir = 0;  // 0：向两侧扩散，1：向中间回收
    static s8 spread = -1; // -1：中间三个灯，0..8：向两侧扩散/回收
    const s8 center_idx = 10; // 中间位置
    const s8 max_spread = 8; // 扩散到边界位置
    u8 i;

    cnt++;
    if (cnt < 500) {
        return;
    }
    cnt = 0;

    // 清空当前所有发动机转速边框灯
    for (i = 0; i < ARRAY_SIZE(engine_speed_frame_map); i++) {
        aip3368h_display_engine_speed_frame_by_idx(0, i);
    }

    if (0 == dir) {
        if (spread < 0) {
            // 从中间三个灯开始
            aip3368h_display_engine_speed_frame_by_idx(1, center_idx - 1);
            aip3368h_display_engine_speed_frame_by_idx(1, center_idx);
            aip3368h_display_engine_speed_frame_by_idx(1, center_idx + 1);
            spread = 0;
        } else {
            // 两个灯往左侧移动，两个灯往右侧移动
            aip3368h_display_engine_speed_frame_by_idx(1, center_idx - 1 - spread);
            aip3368h_display_engine_speed_frame_by_idx(1, center_idx - 2 - spread);
            aip3368h_display_engine_speed_frame_by_idx(1, center_idx + 1 + spread);
            aip3368h_display_engine_speed_frame_by_idx(1, center_idx + 2 + spread);

            if (spread >= max_spread) {
                dir = 1;
                spread = max_spread - 1;
            } else {
                spread++;
            }
        }
    } else {
        if (spread < 0) {
            // 回收到中间时，仍然保持三个中心灯的状态
            aip3368h_display_engine_speed_frame_by_idx(1, center_idx - 1);
            aip3368h_display_engine_speed_frame_by_idx(1, center_idx);
            aip3368h_display_engine_speed_frame_by_idx(1, center_idx + 1);
            dir = 0;
            spread = 0;
        } else {
            // 碰到边界后，直接开始向中间回收，不再多停一拍
            aip3368h_display_engine_speed_frame_by_idx(1, center_idx - 1 - spread);
            aip3368h_display_engine_speed_frame_by_idx(1, center_idx - 2 - spread);
            aip3368h_display_engine_speed_frame_by_idx(1, center_idx + 1 + spread);
            aip3368h_display_engine_speed_frame_by_idx(1, center_idx + 2 + spread);

            if (spread <= 0) {
                dir = 0;
                spread = -1;
            } else {
                spread--;
            }
        }
    }
}
 
void test_aip3368h_display_fuel_lev_1ms_isr(void)
{
    static u16 cnt = 0; // 计数，用于控制显示的频率
    static u8 fuel_lev = 0;
    cnt++;
    if (cnt >= 500) {
        cnt = 0;
        aip3368h_display_fuel_lev(1, fuel_lev);
        fuel_lev++;
        if (fuel_lev >= ARRAY_SIZE(fuel_lev_map) + 1) {
            fuel_lev = 0;
        }
    }
}

#define TEST_AIP3368H_DISPLAY_MILEAGE_BIT_X_ENABLE 0
#define TEST_AIP3368H_DISPLAY_MILEAGE_ENABLE       1
#if (TEST_AIP3368H_DISPLAY_MILEAGE_BIT_X_ENABLE &&                             \
     TEST_AIP3368H_DISPLAY_MILEAGE_ENABLE)
#error can't be enabled at the same time"
#endif
void test_aip3368h_display_mileage_1ms_isr(void)
{
#if TEST_AIP3368H_DISPLAY_MILEAGE_BIT_X_ENABLE
    static u16 cnt = 0; // 计数，用于控制显示的频率
    static u8 num = 0;
    u8 i;
    cnt++;
    if (cnt < 500) {
        return;
    } else {
        cnt = 0;
    }

    // 10 用来控制数码管不显示
    for (i = 0; i < 5; i++) {
        if (num == 10) {
            aip3368h_display_mileage_bit_x(0, i, num);
        } else {
            aip3368h_display_mileage_bit_x(1, i, num);
        }
    }

    num++;
    if (num > 10) {
        num = 0;
    }
#endif

#if TEST_AIP3368H_DISPLAY_MILEAGE_ENABLE
    const u32 buf[] = {1, 9, 10, 12, 123, 1234, 12345, 99999};
    static u16 cnt = 0; // 计数，用于控制显示的频率
    static u8 idx = 0;
    cnt++;
    if (cnt < 1000) {
        return;
    } else {
        cnt = 0;
    }

    aip3368h_display_mileage(buf[idx]);
    idx++;
    if (idx >= ARRAY_SIZE(buf)) {
        idx = 0;
    }

#endif
}

void test_aip3368h_display_time_1ms_isr(void)
{
    static u8 cnt = 0; // 计数，用于控制显示的频率
    static u8 minute = 0;
    static u8 hour = 0;
    cnt++;
    if (cnt < 200) {
        return;
    } else {
        cnt = 0;
    }

    // aip3368h_display_time_minute(1, minute);
    // minute++;
    // if (minute > 99) {
    //     minute = 0;
    // }

    aip3368h_display_time_hour(1, hour);
    hour++;
    if (hour > 23) {
        hour = 0;
    }
}
#endif

void test_aip3368h_display(void)
{
    // aip3368h_display_buff[0] |= 0x01 << 0; // 发动机转速，8.5 对应的指示灯
    // aip3368h_display_buff[0] |= 0x01 << 1; // 发动机转速，9 对应的指示灯
    // aip3368h_display_buff[0] |= 0x01 << 2; // 发动机转速，9.5 对应的指示灯
    // aip3368h_display_buff[0] |= 0x01 << 3; // 发动机转速，10 对应的指示灯
    // aip3368h_display_buff[0] |= 0x01 << 4; // 发动机转速，10.5 对应的指示灯
    // aip3368h_display_buff[0] |= 0x01 << 5; // 发动机转速，11 对应的指示灯
    // aip3368h_display_buff[0] |= 0x01 << 6; // 发动机转速，11.5 对应的指示灯
    // aip3368h_display_buff[0] |= 0x01 << 7; // 发动机转速，12 对应的指示灯
    // aip3368h_display_buff[0] |= 0x01 << 8; // 发动机转速边框，第 16 个指示灯(从左往右，从0开始)
    // aip3368h_display_buff[0] |= 0x01 << 9; // 发动机转速边框，第 17 个指示灯(从左往右，从0开始)
    // aip3368h_display_buff[0] |= 0x01 << 10; // 发动机转速边框，第 18 个指示灯(从左往右，从0开始)
    // aip3368h_display_buff[0] |= 0x01 << 11; // 发动机转速边框，第 19 个指示灯(从左往右，从0开始)
    // aip3368h_display_buff[0] |= 0x01 << 12; // 发动机转速边框，第 20 个指示灯(从左往右，从0开始)
    // aip3368h_display_buff[0] |= 0x01 << 13; // NC
    // aip3368h_display_buff[0] |= 0x01 << 14; // NC
    // aip3368h_display_buff[0] |= 0x01 << 15; // NC

    // aip3368h_display_buff[1] |= 0x01 << 0; // 发动机转速，0.5 对应的指示灯
    // aip3368h_display_buff[1] |= 0x01 << 1; // 发动机转速，1 对应的指示灯
    // aip3368h_display_buff[1] |= 0x01 << 2; // 发动机转速，1.5 对应的指示灯
    // aip3368h_display_buff[1] |= 0x01 << 3; // 发动机转速，2 对应的指示灯
    // aip3368h_display_buff[1] |= 0x01 << 4; // 发动机转速，2.5 对应的指示灯
    // aip3368h_display_buff[1] |= 0x01 << 5; // 发动机转速，3 对应的指示灯
    // aip3368h_display_buff[1] |= 0x01 << 6; // 发动机转速，3.5 对应的指示灯
    // aip3368h_display_buff[1] |= 0x01 << 7; // 发动机转速，4 对应的指示灯
    // aip3368h_display_buff[1] |= 0x01 << 8; // 发动机转速，4.5 对应的指示灯
    // aip3368h_display_buff[1] |= 0x01 << 9; // 发动机转速，5 对应的指示灯
    // aip3368h_display_buff[1] |= 0x01 << 10; // 发动机转速，5.5 对应的指示灯
    // aip3368h_display_buff[1] |= 0x01 << 11; // 发动机转速，6 对应的指示灯
    // aip3368h_display_buff[1] |= 0x01 << 12; // 发动机转速，6.5 对应的指示灯
    // aip3368h_display_buff[1] |= 0x01 << 13; // 发动机转速，7 对应的指示灯
    // aip3368h_display_buff[1] |= 0x01 << 14; // 发动机转速，7.5 对应的指示灯
    // aip3368h_display_buff[1] |= 0x01 << 15; // 发动机转速，8 对应的指示灯

    // aip3368h_display_buff[2] |= 0x01 << 0; // 发动机转速，刻度 0 对应的指示灯
    // aip3368h_display_buff[2] |= 0x01 << 1; // 发动机转速，刻度 1 对应的指示灯
    // aip3368h_display_buff[2] |= 0x01 << 2; // 发动机转速，刻度 2 对应的指示灯
    // aip3368h_display_buff[2] |= 0x01 << 3; // 发动机转速，刻度 3 对应的指示灯
    // aip3368h_display_buff[2] |= 0x01 << 4; // 发动机转速，刻度 4 对应的指示灯
    // aip3368h_display_buff[2] |= 0x01 << 5; // 发动机转速，刻度 5 对应的指示灯
    // aip3368h_display_buff[2] |= 0x01 << 6; // 发动机转速，刻度 6 对应的指示灯
    // aip3368h_display_buff[2] |= 0x01 << 7; // 发动机转速，刻度 7 对应的指示灯
    // aip3368h_display_buff[2] |= 0x01 << 8; // 发动机转速，刻度 8 对应的指示灯
    // aip3368h_display_buff[2] |= 0x01 << 9; // 发动机转速，刻度 9 对应的指示灯
    // aip3368h_display_buff[2] |= 0x01 << 10; // 发动机转速，刻度 10 对应的指示灯
    // aip3368h_display_buff[2] |= 0x01 << 11; // 发动机转速，刻度 11 对应的指示灯
    // aip3368h_display_buff[2] |= 0x01 << 12; // 发动机转速，刻度 12 对应的指示灯
    // aip3368h_display_buff[2] |= 0x01 << 13; // x1000rpm字样对应的指示灯，第 2 个(从左往右，从0开始)
    // aip3368h_display_buff[2] |= 0x01 << 14; // x1000rpm字样对应的指示灯，第 1 个(从左往右，从0开始)
    // aip3368h_display_buff[2] |= 0x01 << 15; // x1000rpm字样对应的指示灯，第 1 个(从左往右，从0开始)

    // aip3368h_display_buff[3] |= 0x01 << 0; // 发动机转速边框，第 0 个指示灯(从左往右，从0开始)
    // aip3368h_display_buff[3] |= 0x01 << 1; // 发动机转速边框，第 1 个指示灯(从左往右，从0开始)
    // aip3368h_display_buff[3] |= 0x01 << 2; // 发动机转速边框，第 2 个指示灯(从左往右，从0开始)
    // aip3368h_display_buff[3] |= 0x01 << 3; // 发动机转速边框，第 3 个指示灯(从左往右，从0开始)
    // aip3368h_display_buff[3] |= 0x01 << 4; // 发动机转速边框，第 4 个指示灯(从左往右，从0开始)
    // aip3368h_display_buff[3] |= 0x01 << 5; // 发动机转速边框，第 5 个指示灯(从左往右，从0开始)
    // aip3368h_display_buff[3] |= 0x01 << 6; // 发动机转速边框，第 6 个指示灯(从左往右，从0开始)
    // aip3368h_display_buff[3] |= 0x01 << 7; // 发动机转速边框，第 7 个指示灯(从左往右，从0开始)
    // aip3368h_display_buff[3] |= 0x01 << 8; // 发动机转速边框，第 8 个指示灯(从左往右，从0开始)
    // aip3368h_display_buff[3] |= 0x01 << 9; // 发动机转速边框，第 9 个指示灯(从左往右，从0开始)
    // aip3368h_display_buff[3] |= 0x01 << 10; // 发动机转速边框，第 10 个指示灯(从左往右，从0开始)
    // aip3368h_display_buff[3] |= 0x01 << 11; // 发动机转速边框，第 11 个指示灯(从左往右，从0开始)
    // aip3368h_display_buff[3] |= 0x01 << 12; // 发动机转速边框，第 12 个指示灯(从左往右，从0开始)
    // aip3368h_display_buff[3] |= 0x01 << 13; // 发动机转速边框，第 13 个指示灯(从左往右，从0开始)
    // aip3368h_display_buff[3] |= 0x01 << 14; // 发动机转速边框，第 14 个指示灯(从左往右，从0开始)
    // aip3368h_display_buff[3] |= 0x01 << 15; // 发动机转速边框，第 15 个指示灯(从左往右，从0开始)

    // aip3368h_display_buff[4] |= 0x01 << 0;  // 挡位 a 段指示灯
    // aip3368h_display_buff[4] |= 0x01 << 1;  // 挡位 b 段指示灯
    // aip3368h_display_buff[4] |= 0x01 << 2;  // 挡位 c 段指示灯
    // aip3368h_display_buff[4] |= 0x01 << 3;  // 挡位 d 段指示灯
    // aip3368h_display_buff[4] |= 0x01 << 4;  // 挡位 e 段指示灯
    // aip3368h_display_buff[4] |= 0x01 << 5;  // 挡位 f 段指示灯
    // aip3368h_display_buff[4] |= 0x01 << 6;  // 挡位 g 段指示灯
    // aip3368h_display_buff[4] |= 0x01 << 7; // 挡位边框第 0 个指示灯，从右下开始，逆时针排序
    // aip3368h_display_buff[4] |= 0x01 << 8; // 挡位边框第 1 个指示灯，从右下开始，逆时针排序
    // aip3368h_display_buff[4] |= 0x01 << 9; // 挡位边框第 2 个指示灯，从右下开始，逆时针排序
    // aip3368h_display_buff[4] |= 0x01 << 10; // 挡位边框第 3 个指示灯，从右下开始，逆时针排序
    // aip3368h_display_buff[4] |= 0x01 << 11; // 挡位边框第 4 个指示灯，从右下开始，逆时针排序
    // aip3368h_display_buff[4] |= 0x01 << 12; // 挡位边框第 5 个指示灯，从右下开始，逆时针排序
    // aip3368h_display_buff[4] |= 0x01 << 13; // 挡位边框第 6 个指示灯，从右下开始，逆时针排序
    // aip3368h_display_buff[4] |= 0x01 << 14; // 挡位边框第 7 个指示灯，从右下开始，逆时针排序
    // aip3368h_display_buff[4] |= 0x01 << 15; // 挡位边框第 8 个指示灯，从右下开始，逆时针排序

    // aip3368h_display_buff[5] |= 0x01 << 0; // 右转向指示灯
    // aip3368h_display_buff[5] |= 0x01 << 1; // 远光指示灯
    // aip3368h_display_buff[5] |= 0x01 << 2; // 时速第 2 位， b 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[5] |= 0x01 << 3; // 时速第 2 位， c 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[5] |= 0x01 << 4; // NC 灯没有使用
    // aip3368h_display_buff[5] |= 0x01 << 5; // NC 灯没有使用
    // aip3368h_display_buff[5] |= 0x01 << 6; // NC 灯没有使用
    // aip3368h_display_buff[5] |= 0x01 << 7; // NC 灯没有使用
    // aip3368h_display_buff[5] |= 0x01 << 8; // NC
    // aip3368h_display_buff[5] |= 0x01 << 9; // NC 灯没有使用
    // aip3368h_display_buff[5] |= 0x01 << 10; // N挡图标对应的指示灯
    // aip3368h_display_buff[5] |= 0x01 << 11; // ABS图标对应的指示灯
    // aip3368h_display_buff[5] |= 0x01 << 12; // 低电量提示图标对应的指示灯
    // aip3368h_display_buff[5] |= 0x01 << 13; // 发动机故障指示灯
    // aip3368h_display_buff[5] |= 0x01 << 14; // mph 字样对应的指示灯
    // aip3368h_display_buff[5] |= 0x01 << 15; // km/h 字样对应的指示灯

    // aip3368h_display_buff[6] |= 0x01 << 0; // 时速第 0 位， a 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[6] |= 0x01 << 1; // 时速第 0 位， b 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[6] |= 0x01 << 2; // 时速第 0 位， c 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[6] |= 0x01 << 3; // 时速第 0 位， d 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[6] |= 0x01 << 4; // 时速第 0 位， e 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[6] |= 0x01 << 5; // 时速第 0 位， f 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[6] |= 0x01 << 6; // 时速第 0 位， g 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[6] |= 0x01 << 7; // NC
    // aip3368h_display_buff[6] |= 0x01 << 8; // 挡位边框第 9 个指示灯，从右下开始，逆时针排序
    // aip3368h_display_buff[6] |= 0x01 << 9; // 时速第 1 位， a 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[6] |= 0x01 << 10; // 时速第 1 位， b 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[6] |= 0x01 << 11; // 时速第 1 位， c 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[6] |= 0x01 << 12; // 时速第 1 位， d 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[6] |= 0x01 << 13; // 时速第 1 位， e 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[6] |= 0x01 << 14; // 时速第 1 位， f 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[6] |= 0x01 << 15; // 时速第 1 位， g 段指示灯(位从右往左，从0开始)

    // aip3368h_display_buff[7] |= 0x01 << 0; // 分钟个位，a 段指示灯
    // aip3368h_display_buff[7] |= 0x01 << 1; // 分钟个位，b 段指示灯
    // aip3368h_display_buff[7] |= 0x01 << 2; // 分钟个位，c 段指示灯
    // aip3368h_display_buff[7] |= 0x01 << 3; // 分钟个位，d 段指示灯
    // aip3368h_display_buff[7] |= 0x01 << 4; // 分钟个位，e 段指示灯
    // aip3368h_display_buff[7] |= 0x01 << 5; // 分钟个位，f 段指示灯
    // aip3368h_display_buff[7] |= 0x01 << 6; // 分钟个位，g 段指示灯
    // aip3368h_display_buff[7] |= 0x01 << 7; // NC
    // aip3368h_display_buff[7] |= 0x01 << 8; // NC
    // aip3368h_display_buff[7] |= 0x01 << 9; // 发动机转速， 12 字样对应的指示灯
    // aip3368h_display_buff[7] |= 0x01 << 10; // 发动机转速， 10 字样对应的指示灯
    // aip3368h_display_buff[7] |= 0x01 << 11; // 发动机转速， 8 字样对应的指示灯
    // aip3368h_display_buff[7] |= 0x01 << 12; // 发动机转速， 6 字样对应的指示灯
    // aip3368h_display_buff[7] |= 0x01 << 13; // 发动机转速， 4 字样对应的指示灯
    // aip3368h_display_buff[7] |= 0x01 << 14; // 发动机转速， 2 字样对应的指示灯
    // aip3368h_display_buff[7] |= 0x01 << 15; // 发动机转速， 0 字样对应的指示灯

    // aip3368h_display_buff[8] |= 0x01 << 0; // 分钟十位，a 段指示灯
    // aip3368h_display_buff[8] |= 0x01 << 1; // 分钟十位，b 段指示灯
    // aip3368h_display_buff[8] |= 0x01 << 2; // 分钟十位，c 段指示灯
    // aip3368h_display_buff[8] |= 0x01 << 3; // 分钟十位，d 段指示灯
    // aip3368h_display_buff[8] |= 0x01 << 4; // 分钟十位，e 段指示灯
    // aip3368h_display_buff[8] |= 0x01 << 5; // 分钟十位，f 段指示灯
    // aip3368h_display_buff[8] |= 0x01 << 6; // 分钟十位，g 段指示灯
    // aip3368h_display_buff[8] |= 0x01 << 7; // 时间分隔符对应的指示灯
    // aip3368h_display_buff[8] |= 0x01 << 8; // NC
    // aip3368h_display_buff[8] |= 0x01 << 9; // 小时个位，a 段指示灯
    // aip3368h_display_buff[8] |= 0x01 << 10; // 小时个位，b 段指示灯
    // aip3368h_display_buff[8] |= 0x01 << 11; // 小时个位，c 段指示灯
    // aip3368h_display_buff[8] |= 0x01 << 12; // 小时个位，d 段指示灯
    // aip3368h_display_buff[8] |= 0x01 << 13; // 小时个位，e 段指示灯
    // aip3368h_display_buff[8] |= 0x01 << 14; // 小时个位，f 段指示灯
    // aip3368h_display_buff[8] |= 0x01 << 15; // 小时个位，g 段指示灯

    // aip3368h_display_buff[9] |= 0x01 << 0; // km 字样对应的指示灯
    // aip3368h_display_buff[9] |= 0x01 << 1; // 油量 E 字样对应的指示灯
    // aip3368h_display_buff[9] |= 0x01 << 2; // NC
    // aip3368h_display_buff[9] |= 0x01 << 3; // NC
    // aip3368h_display_buff[9] |= 0x01 << 4; // NC
    // aip3368h_display_buff[9] |= 0x01 << 5; // NC
    // aip3368h_display_buff[9] |= 0x01 << 6; // 时钟图标对应的指示灯
    // aip3368h_display_buff[9] |= 0x01 << 7; // 左转向指示灯
    // aip3368h_display_buff[9] |= 0x01 << 8; // 近光灯指示灯
    // aip3368h_display_buff[9] |= 0x01 << 9; // 小时十位， a 段指示灯
    // aip3368h_display_buff[9] |= 0x01 << 10; // 小时十位， b 段指示灯
    // aip3368h_display_buff[9] |= 0x01 << 11; // 小时十位， c 段指示灯
    // aip3368h_display_buff[9] |= 0x01 << 12; // 小时十位， d 段指示灯
    // aip3368h_display_buff[9] |= 0x01 << 13; // 小时十位， e 段指示灯
    // aip3368h_display_buff[9] |= 0x01 << 14; // 小时十位， f 段指示灯
    // aip3368h_display_buff[9] |= 0x01 << 15; // 小时十位， g 段指示灯

    // aip3368h_display_buff[10] |= 0x01 << 0; // 里程第 2 位，a 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[10] |= 0x01 << 1; // 里程第 2 位，b 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[10] |= 0x01 << 2; // 里程第 2 位，c 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[10] |= 0x01 << 3; // 里程第 2 位，d 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[10] |= 0x01 << 4; // 里程第 2 位，e 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[10] |= 0x01 << 5; // 里程第 2 位，f 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[10] |= 0x01 << 6; // 里程第 2 位，g 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[10] |= 0x01 << 7; // TRIP 字样对应的指示灯
    // aip3368h_display_buff[10] |= 0x01 << 8; // ODO 字样对应的指示灯
    // aip3368h_display_buff[10] |= 0x01 << 9; // 里程第 3 位，a 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[10] |= 0x01 << 10; // 里程第 3 位，b 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[10] |= 0x01 << 11; // 里程第 3 位，c 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[10] |= 0x01 << 12; // 里程第 3 位，d 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[10] |= 0x01 << 13; // 里程第 3 位，e 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[10] |= 0x01 << 14; // 里程第 3 位，f 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[10] |= 0x01 << 15; // 里程第 3 位，g 段指示灯(位从右往左，从0开始)

    // aip3368h_display_buff[11] |= 0x01 << 0; // 里程第 0 位，a 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[11] |= 0x01 << 1; // 里程第 0 位，b 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[11] |= 0x01 << 2; // 里程第 0 位，c 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[11] |= 0x01 << 3; // 里程第 0 位，d 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[11] |= 0x01 << 4; // 里程第 0 位，e 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[11] |= 0x01 << 5; // 里程第 0 位，f 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[11] |= 0x01 << 6; // 里程第 0 位，g 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[11] |= 0x01 << 7; // mile 字样对应的指示灯
    // aip3368h_display_buff[11] |= 0x01 << 8; // 里程小数点对应的指示灯
    // aip3368h_display_buff[11] |= 0x01 << 9; // 里程第 1 位，a 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[11] |= 0x01 << 10; // 里程第 1 位，b 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[11] |= 0x01 << 11; // 里程第 1 位，c 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[11] |= 0x01 << 12; // 里程第 1 位，d 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[11] |= 0x01 << 13; // 里程第 1 位，e 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[11] |= 0x01 << 14; // 里程第 1 位，f 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[11] |= 0x01 << 15; // 里程第 1 位，g 段指示灯(位从右往左，从0开始)

    // aip3368h_display_buff[12] |= 0x01 << 0; // 油量第 0 格指示灯(红灯)
    // aip3368h_display_buff[12] |= 0x01 << 1; // 油量第 0 格指示灯(蓝灯)
    // aip3368h_display_buff[12] |= 0x01 << 2; // 油量第 1 格指示灯(蓝)
    // aip3368h_display_buff[12] |= 0x01 << 3; // 油量第 2 格指示灯(蓝)
    // aip3368h_display_buff[12] |= 0x01 << 4; // 油量第 3 格指示灯(蓝)
    // aip3368h_display_buff[12] |= 0x01 << 5; // 油量第 4 格指示灯(蓝)
    // aip3368h_display_buff[12] |= 0x01 << 6; // 油量第 5 格指示灯(蓝)
    // aip3368h_display_buff[12] |= 0x01 << 7; // 油桶图标对应的指示灯
    // aip3368h_display_buff[12] |= 0x01 << 8; // 油量 F 字样对应的指示灯
    // aip3368h_display_buff[12] |= 0x01 << 9; // 里程第 4 位， a 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[12] |= 0x01 << 10; // 里程第 4 位， b 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[12] |= 0x01 << 11; // 里程第 4 位， c 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[12] |= 0x01 << 12; // 里程第 4 位， d 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[12] |= 0x01 << 13; // 里程第 4 位， e 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[12] |= 0x01 << 14; // 里程第 4 位， f 段指示灯(位从右往左，从0开始)
    // aip3368h_display_buff[12] |= 0x01 << 15; // 里程第 4 位， g 段指示灯(位从右往左，从0开始)
}
#endif