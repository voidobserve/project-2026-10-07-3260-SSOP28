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
    以下是“位置表”：每一项描述 1 位数码管（或 1 段连续排列的灯）在显存中的起点，
    不再逐段列出 a ~ g，省掉 6/7 的常量空间。

    bit_offset 的含义：
        - 数码管：该位 a 段所在的位（b 段 = bit_offset + 1 ... g 段 = bit_offset + 6）
        - 连续段的灯：该段第 0 个灯所在的位，每段最多 16 个（即一个显存元素的 0 ~ 15 位）
*/

// 时间：小时个位、小时十位、分钟个位、分钟十位
static const aip3368h_display_mapping_t time_digit_map[] = {
    {8, 9}, // 小时个位
    {9, 9}, // 小时十位
    {7, 0}, // 分钟个位
    {8, 0}, // 分钟十位
};

// 里程：第 0 ~ 4 位数码管（从右往左）
static const aip3368h_display_mapping_t mileage_map[] = {
    {11, 0}, {11, 9}, {10, 0}, {10, 9}, {12, 9},
};

// 时速：第 0、1 位数码管（从右往左）
static const aip3368h_display_mapping_t speed_digit_map[] = {
    {6, 0},
    {6, 9},
};

// 挡位：只有 1 位数码管
const aip3368h_display_mapping_t gear_map[] = {
    {4, 0},
};

// 发动机转速边框灯：第 0 段 {3,0} 共 16 个，第 1 段 {0,8} 共 5 个
const aip3368h_display_mapping_t engine_speed_frame_seg_map[] = {
    {3, 0},
    {0, 8},
};

// 发动机转速格数灯：第 0 段 {1,0} 共 16 个，第 1 段 {0,0} 共 8 个
static const aip3368h_display_mapping_t engine_speed_lev_seg_map[] = {
    {1, 0},
    {0, 0},
};

// 油量格数灯（第 0 格红色灯之后，共 6 格，连续排列）
#define FUEL_LEV_SEG_BUFF_INDEX ((u8)12)
#define FUEL_LEV_SEG_BASE_BIT   ((u8)1)
#define FUEL_LEV_NUMS           ((u8)6)

/**
 * @brief 设置显存中的单个 bit（指示灯 / 数码管的某一段）
 *
 * @param buff_index 显存数组下标
 * @param bit_offset 显存元素中的第 x 位（从右往左，从 0 开始）
 * @param is_display 1：点亮，0：熄灭
 */
static void __aip3368h_display_set_bit__(u8 buff_index, u8 bit_offset,
                                          u8 is_display)
{
    if (is_display) {
        AIP3368H_DISPLAY_BUF_SET(buff_index, bit_offset);
    } else {
        AIP3368H_DISPLAY_BUF_CLR(buff_index, bit_offset);
    }
}

/**
 * @brief 按段码显示一位数码管
 *
 * @param pos         该位数码管在显存中的起点（a 段位置）
 * @param segment_code 段码，bit0=a ... bit6=g
 * @param is_display  1：显示，0：清空该位
 */
static void __aip3368h_display_digit__(const aip3368h_display_mapping_t *pos,
                                       u8 segment_code, u8 is_display)
{
    u8 i;

    for (i = 0; i < 7; i++) {
        __aip3368h_display_set_bit__(
            pos->buff_index, (u8)(pos->bit_offset + i),
            is_display && (segment_code & (0x01 << i)));
    }
}

/**
 * @brief 取“连续排列的灯”中第 idx 个灯在显存中的位置
 *
 * @param seg_map     各段起点表，每段最多 16 个灯
 * @param idx         灯的序号，从 0 开始
 * @param buff_index  取出的显存下标
 * @param bit_offset  取出的位号
 */
static void __aip3368h_display_strip_pos__(
    const aip3368h_display_mapping_t *seg_map, u8 idx, u8 *buff_index,
    u8 *bit_offset)
{
    // 每段 16 个灯，段号 = idx / 16，段内偏移 = idx % 16
    const aip3368h_display_mapping_t *seg = &seg_map[idx / 16];

    *buff_index = seg->buff_index;
    *bit_offset = (u8)(seg->bit_offset + (idx % 16));
}

void aip3368h_display_left_turn_light(u8 is_display)
{
    __aip3368h_display_set_bit__(9, 7, is_display);
}

void aip3368h_display_right_turn_light(u8 is_display)
{
    __aip3368h_display_set_bit__(5, 0, is_display);
}

void aip3368h_display_high_beam_light(u8 is_display)
{
    __aip3368h_display_set_bit__(5, 1, is_display);
}

void aip3368h_display_low_beam_light(u8 is_display)
{
    __aip3368h_display_set_bit__(9, 8, is_display);
}

// 未使用该函数
#if 0
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
#endif

/**
 * @brief 显示小时
 *      24小时制，小于10小时，不显示十位数，大于等于10小时，显示十位数
 * 
 * @param is_display 
 *      0：不显示 ，后续的参数无效
 *      1：显示
 * 
 * @param hour 
 */
void aip3368h_display_time_hour(u8 is_display, u8 hour)
{
    if (is_display) {
        // 小时个位
        __aip3368h_display_digit__(&time_digit_map[0],
                                   digit_segment_code[hour % 10], 1);

        if (hour >= 10) {
            // 小时十位
            __aip3368h_display_digit__(&time_digit_map[1],
                                       digit_segment_code[hour / 10], 1);
        } else {
            // hour < 10，清空小时的十位显示
            __aip3368h_display_digit__(&time_digit_map[1], 0, 0);
        }
    } else {
        // 清空小时个位、十位的显示
        __aip3368h_display_digit__(&time_digit_map[0], 0, 0);
        __aip3368h_display_digit__(&time_digit_map[1], 0, 0);
    }
}

/**
 * @brief 显示分钟
 * 
 * @param is_display 
 *      0：不显示 ，后续的参数无效
 *      1：显示
 * 
 * @param minute 
 */
void aip3368h_display_time_minute(u8 is_display, u8 minute)
{
    if (is_display) {
        // 分钟个位
        __aip3368h_display_digit__(&time_digit_map[2],
                                   digit_segment_code[minute % 10], 1);
        // 分钟十位
        __aip3368h_display_digit__(&time_digit_map[3],
                                   digit_segment_code[minute / 10], 1);
    } else {
        // 清空分钟个位、十位的显示
        __aip3368h_display_digit__(&time_digit_map[2], 0, 0);
        __aip3368h_display_digit__(&time_digit_map[3], 0, 0);
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
    __aip3368h_display_set_bit__(8, 7, is_display);
}

/**
 * @brief 里程的小数点
 *
 * @param is_display
 */
void aip3368h_display_mileage_point_light(u8 is_display)
{
    __aip3368h_display_set_bit__(11, 8, is_display);
}

/**
 * @brief 里程 TRIP 字样对应的指示灯
 *
 * @param is_display
 */
void aip3368h_display_trip_light(u8 is_display)
{
    __aip3368h_display_set_bit__(10, 7, is_display);
}

/**
 * @brief 里程 ODO 字样对应的指示灯
 *
 */
void aip3368h_display_odo_light(u8 is_display)
{
    __aip3368h_display_set_bit__(10, 8, is_display);
}

/**
 * @brief 里程 km 字样对应的指示灯
 *
 * @param is_display
 */
void aip3368h_display_km_light(u8 is_display)
{
    __aip3368h_display_set_bit__(9, 0, is_display);
}

/**
 * @brief 里程 mile 字样对应的指示灯
 *
 * @param is_display
 */
void aip3368h_display_mile_light(u8 is_display)
{
    __aip3368h_display_set_bit__(11, 7, is_display);
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
    __aip3368h_display_digit__(&mileage_map[bit_x], digit_segment_code[num],
                               is_display);
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
    for (i = 0; i < ARRAY_SIZE(mileage_map); i++) {
        // i 从 0 开始，按位数直接判断即可；未显示位要清空对应的数码管
        if (i < valid_bits) {
            aip3368h_display_mileage_bit_x(1, i, (mileage % 10));
        } else {
            aip3368h_display_mileage_bit_x(0, i, 0);
        }

        mileage /= 10;
    }
}

// 未使用该函数
#if 0
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
#endif

// 未使用该函数
#if 0
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
#endif

/**
 * @brief 油量图标对应的指示灯
 * 
 */
void aip3368h_display_fuel_icon_light(u8 is_display)
{
    __aip3368h_display_set_bit__(12, 7, is_display);
}

/**
 * @brief 油量第 0 格，红色指示灯
 *
 */
void aip3368h_display_fuel_lev_0_red_light(u8 is_display)
{
    __aip3368h_display_set_bit__(12, 0, is_display);
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
void aip3368h_display_fuel_lev(u8 fuel_lev)
{
    u8 i;

    for (i = 0; i < FUEL_LEV_NUMS; i++) {
        __aip3368h_display_set_bit__((u8)(FUEL_LEV_SEG_BUFF_INDEX),
                                     (u8)(FUEL_LEV_SEG_BASE_BIT + i),
                                     (i < fuel_lev));
    }
}

/**
 * @brief 根据索引，点亮发动机转速边框对应的指示灯
 *
 */
void aip3368h_display_engine_speed_frame_by_idx(u8 is_display, u8 idx)
{
    u8 buff_index;
    u8 bit_offset;

    __aip3368h_display_strip_pos__(engine_speed_frame_seg_map, idx, &buff_index,
                                   &bit_offset);
    __aip3368h_display_set_bit__(buff_index, bit_offset, is_display);
}

/**
 * @brief 显示发动机转速
 *  
 * @param engine_speed_lev 发动机转速等级  
 *          0：不显示
 *          1：显示 1 格
 *          2：显示 2 格
 *          ...
 * 
 */
void aip3368h_display_engine_speed_lev(u16 engine_speed_lev)
{
    u8 i;
    u8 buff_index;
    u8 bit_offset;

    for (i = 0; i < AIP3368H_DISPLAY_ENGINE_SPEED_LEV_NUMS; i++) {
        __aip3368h_display_strip_pos__(engine_speed_lev_seg_map, i, &buff_index,
                                       &bit_offset);
        __aip3368h_display_set_bit__(buff_index, bit_offset,
                                     (i < engine_speed_lev));
    }
}

// n挡图标对应的指示灯
void aip3368h_display_gear_n_light(u8 is_display)
{
    __aip3368h_display_set_bit__(5, 10, is_display);
}

/**
 * @brief 
 * 
 * @param is_display 
 * @param gear 
 *      0： N挡
 *      1： 1挡
 *      ...
 *      6： 6挡
 *      255: 没有挡位，会清空所有挡位的显示
 */
void aip3368h_display_gear(u8 gear)
{
    aip3368h_display_gear_n_light(0);
    // 清空挡位对应的 a ~ g 段
    __aip3368h_display_digit__(&gear_map[0], 0, 0);

    if (0 == gear) {
        aip3368h_display_gear_n_light(1);
    } else if (gear <= 6) {
        __aip3368h_display_digit__(&gear_map[0], digit_segment_code[gear], 1);
    }
}

void aip3368h_display_mph_light(u8 is_display)
{
    __aip3368h_display_set_bit__(5, 14, is_display);
}

void aip3368h_display_kmh_light(u8 is_display)
{
    __aip3368h_display_set_bit__(5, 15, is_display);
}

/**
 * @brief 时速对应的数据位
 * 
 * @param is_display 
 *         0： 不显示，参数 bit_value 无效
 *         1： 显示
 * @param bit_index 0 ~ 2
 *         0： 第 0 位，个位
 *         1： 第 1 位，十位
 *         2： 第 2 位，百位，参数 bit_value 无效，百位只有b段和c段数码管
 * @param bit_value 0 ~ 9
 */
void aip3368h_display_speed_bit(u8 is_display, u8 bit_index, u8 bit_value)
{
    if (bit_index < 2) {
        // 第 0 位、第 1 位使用段码
        __aip3368h_display_digit__(&speed_digit_map[bit_index],
                                   digit_segment_code[bit_value], is_display);
        return;
    }

    // 百位只有 b 段和 c 段数码管，不使用段码
    __aip3368h_display_set_bit__(5, 2, is_display);
    __aip3368h_display_set_bit__(5, 3, is_display);
}

/**
 * @brief 时速数码管显示时速
 *
 * @param speed 0 ~ 199，如果超过了 199，会显示 199
 *      数码管从右往左排序
 *      参数小于 10 ，只有第 0 位数码管在显示
 *      参数小于 100 ，只有第 0、1 位数码管在显示
 *
 */
void aip3368h_display_speed(u8 speed)
{
    /*
        根据传参，先计算有多少位有效数字，
        再根据有效数字进行对应显示
    */
    // 传参的有效数据位：
    u8 valid_bits = 0;
    u16 tmp = 0; // 计算有效数据位使用的临时变量
    u8 i;

    if (speed > 199) {
        // 在这里限制要显示的时速不能超过 199
        speed = 199;
    }
    tmp = speed;
    while (1) {
        valid_bits++; // 刚进入，默认至少有1位有效数据
        tmp /= 10;
        if (tmp == 0) {
            break;
        }
    }

#if USER_DEBUG_ENABLE
    // printf("valid_bits == %u\n", (u16)valid_bits);
#endif

    // 遍历第 0 ~ 2 位数码管
    for (i = 0; i <= 2; i++) {
        if (i < valid_bits) {
            // 遍历第 0 ~ valid_bits（最大为2） 位数码管
            aip3368h_display_speed_bit(1, i, speed % 10);
            speed /= 10;
        } else {
            // 不让当前位对应的数码管进行显示
            aip3368h_display_speed_bit(0, i, 0);
        }
    }
}

void aip3368h_display_abs_light(u8 is_display)
{
    __aip3368h_display_set_bit__(5, 11, is_display);
}

void aip3368h_display_engine_err_light(u8 is_display)
{
    __aip3368h_display_set_bit__(5, 13, is_display);
}

void aip3368h_display_bat_err_light(u8 is_display)
{
    __aip3368h_display_set_bit__(5, 12, is_display);
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
    // aip3368h_display_gear_n_light(is_display);
    // aip3368h_display_mph_light(is_display);
    // aip3368h_display_kmh_light(is_display);
    // aip3368h_display_abs_light(is_display);
    // aip3368h_display_engine_err_light(is_display);
    // aip3368h_display_bat_err_light(is_display);

    is_display = !is_display;
}

#if 0

void test_aip3368h_display_speed_1ms_isr(void)
{
    static u16 cnt = 0; // 计数，用于控制显示的频率
    static u8 speed = 0;
    cnt++;
    if (cnt < 200) {
        return;
    } else {
        cnt = 0;
    }

    aip3368h_display_speed(speed);
    speed++;
    if (speed > 200) {
        speed = 0;
    }
}

void test_aip3368h_display_gear_1ms_isr(void)
{
    static u16 cnt = 0; // 计数，用于控制显示的频率
    static u8 gear = 0;
    cnt++;
    if (cnt < 500) {
        return;
    } else {
        cnt = 0;
    }

    aip3368h_display_gear(gear);
    gear++;
    if (gear > 6) {
        gear = 255;
    } else if (gear == 255) {
        gear = 0;
    }
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

    aip3368h_display_engine_speed_lev( lev);
    lev++;
    if (lev >= AIP3368H_DISPLAY_ENGINE_SPEED_LEV_NUMS + 1) {
        lev = 0;
    }
}

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
        if (idx >= AIP3368H_DISPLAY_ENGINE_SPEED_FRAME_NUMS) {
            idx = AIP3368H_DISPLAY_ENGINE_SPEED_FRAME_NUMS - 1;
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
    static u16 cnt = 0;       // 计数，用于控制显示的频率
    static s8 dir = 0;        // 0：向两侧扩散，1：向中间回收
    static s8 spread = -1;    // -1：中间三个灯，0..8：向两侧扩散/回收
    const s8 center_idx = 10; // 中间位置
    const s8 max_spread = 8;  // 扩散到边界位置
    u8 i;

    cnt++;
    if (cnt < 500) {
        return;
    }
    cnt = 0;

    // 清空当前所有发动机转速边框灯
    for (i = 0; i < AIP3368H_DISPLAY_ENGINE_SPEED_FRAME_NUMS; i++) {
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
            aip3368h_display_engine_speed_frame_by_idx(1,
                                                       center_idx - 1 - spread);
            aip3368h_display_engine_speed_frame_by_idx(1,
                                                       center_idx - 2 - spread);
            aip3368h_display_engine_speed_frame_by_idx(1,
                                                       center_idx + 1 + spread);
            aip3368h_display_engine_speed_frame_by_idx(1,
                                                       center_idx + 2 + spread);

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
            aip3368h_display_engine_speed_frame_by_idx(1,
                                                       center_idx - 1 - spread);
            aip3368h_display_engine_speed_frame_by_idx(1,
                                                       center_idx - 2 - spread);
            aip3368h_display_engine_speed_frame_by_idx(1,
                                                       center_idx + 1 + spread);
            aip3368h_display_engine_speed_frame_by_idx(1,
                                                       center_idx + 2 + spread);

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
        aip3368h_display_fuel_lev(fuel_lev);
        fuel_lev++;
        if (fuel_lev >= FUEL_LEV_NUMS + 1) {
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