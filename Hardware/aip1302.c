#include "aip1302.h"
#include "user_config.h"

#if IC_1302_ENABLE

volatile aip1302_info_t aip1302_info = {0};
volatile u16 aip1302_update_time_interval = 0;

static u8 aip1302_is_working(void); // 函数声明

static u8 __aip1302_read_byte__(const u8 cmd);

/**
 * @brief 配置 DATA 引脚为输出模式
 *
 */
static void aip1302_data_mode_out(void)
{
    // P2_PU &= ~GPIO_P23_PULL_UP(0x01); // 不上拉
    P2_MD0 &= ~GPIO_P23_MODE_SEL(0x3); // 清空配置
    P2_MD0 |= GPIO_P23_MODE_SEL(0x1);  // 输出模式
    FOUT_S23 = GPIO_FOUT_AF_FUNC;
}

/**
 * @brief 配置 DATA 引脚为输入模式
 *
 */
static void aip1302_data_mode_in(void)
{
    P2_PU |= GPIO_P23_PULL_UP(0x01);   // 上拉
    P2_MD0 &= ~GPIO_P23_MODE_SEL(0x3); // 输入模式
}

// aip1302时钟ic初始化
void aip1302_config(void)
{
    u8 ret = 0;

    // aip1302 第一次上电后，必须初始化(--取自规格书)
    // CE脚
    P2_MD0 &= ~GPIO_P21_MODE_SEL(0x3); // 清空配置
    P2_MD0 |= GPIO_P21_MODE_SEL(0x1);  // 输出模式
    FOUT_S21 = GPIO_FOUT_AF_FUNC;

    // SCLK脚
    P2_MD0 &= ~GPIO_P22_MODE_SEL(0x3); // 清空配置
    P2_MD0 |= GPIO_P22_MODE_SEL(0x1);  // 输出模式
    FOUT_S22 = GPIO_FOUT_AF_FUNC;

    // DATA脚
    P2_MD0 &= ~GPIO_P23_MODE_SEL(0x3); // 清空配置
    P2_MD0 |= GPIO_P23_MODE_SEL(0x1);  // 输出模式
    FOUT_S23 = GPIO_FOUT_AF_FUNC;

    AIP1302_CE_PIN = 0;   // 空闲时为低电平
    AIP1302_SCLK_PIN = 0; // 时钟脚空间时为低电平

#if 1
    // 判断时钟芯片aip1302的晶振是否正在运行，
    // 如果在运行，不做任何处理
    // 如果不在运行，将时钟芯片内部的数据配置为 2000.01.01--00:00:00,最后打开晶振
    ret = aip1302_is_working(); // 函数内部调用了 aip1302_read_all();
    if (ret) {
// 如果时钟芯片aip1302的晶振正在运行
#if USER_DEBUG_ENABLE
        printf("aip1302 is working\n");
        printf("1302 is not first power on\n");
#endif
    } else {
// 如果时钟芯片aip1302的晶振不在运行
#if USER_DEBUG_ENABLE
        printf("aip1302 is sleep\n");
        printf("1302 is first power on\n");
#endif
        // aip1302上电复位后，默认不跑时钟，这里要配置它开始跑时钟

        // 设置一个非 2000 年的时间，之后上电读取时，表示不是第一次上电
        aip1302_write_byte(AIP1302_YEAR_REG_ADDR, 10); // 2010年
        aip1302_write_byte(AIP1302_MONTH_REG_ADDR, 1); // 1月
        aip1302_write_byte(AIP1302_DATE_REG_ADDR, 1);  // 1日
        // aip1302_write_byte(AIP1302_WEEKDAY_REG_ADDR, 1); // 星期1
        aip1302_write_byte(AIP1302_MIN_REG_ADDR, 0); // 0分
        // 最高位清零,对应24小时制
        aip1302_write_byte(AIP1302_HOUR_REG_ADDR, 12);
        // 函数内部也会把最高位清零，秒寄存器最高位清零后，时钟ic开始振荡，跑时间
        aip1302_write_byte(AIP1302_SEC_REG_ADDR, 0);

        // aip1302_info.year = (u16)2025;
        // aip1302_info.month = 7;
        // aip1302_info.day = 4;
        // aip1302_info.time_hour = 9;
        // aip1302_info.time_min = 20;
        // aip1302_info.time_sec = 00;
        // aip1302_update_time(aip1302_info);
    }
#endif

    // 从aip1302中读出所有有关时间的数据,存放到全局变量中
    // aip1302_read_all();
    // printf("power on, get aip1302 data: \n");
    // printf("year %u \n", aip1302_info.year);
    // printf("month %bu \n", aip1302_info.month);
    // printf("day %bu \n", aip1302_info.day);
    // printf("hour %bu \n", aip1302_info.time_hour);
    // printf("min %bu \n", aip1302_info.time_min);
    // printf("sec %bu \n", aip1302_info.time_sec);

#if 0
    /* 测试程序 */


    // aip1302_info.year = (u16)2025;
    // aip1302_info.month = 7;
    // aip1302_info.day = 2;
    // aip1302_info.time_hour = 13;
    // aip1302_info.time_min = 30;
    // aip1302_info.time_sec = 30;
    // aip1302_update_time(aip1302_info);
    // aip1302_write_byte(AIP1302_YEAR_REG_ADDR, 25); // 时钟IC内部只存0~99年

    // aip1302_write_byte(AIP1302_RAM_START_ADDR, 0xA5);
    // {
    //     u8 ret = aip1302_read_byte(AIP1302_RAM_START_ADDR + 1);
    //     printf("ret %bu\n", ret);
    // }
#endif
}

// 将8位变量中的数据反转(例：0b 0111 0101  -> 0b 1010 1110)
static u8 reverse_bits_u8(u8 arg)
{
    u8 tmp;   // 临时值，不需要赋值初始值，后续移位会覆盖掉这些未初始化的值
    u8 i = 0; // 循环计数值

    // 直接从aip1302读出的数据是LSB低位在前的，这里先做反转
    for (i = 0; i < 8; i++) {
        tmp <<= 1;
        if (arg & 0x01) {
            tmp |= 0x01;
        }

        arg >>= 1;
    }

    return tmp;
}

// 对读出的数据做格式转换
static u8 __aip1302_convert_read_byte__(const u8 cmd, const u8 byte)
{
    u8 tmp = 0;
    u8 i = 0; // 循环计数值

    // 直接从aip1302读出的数据是LSB低位在前的，这里先做反转
    tmp = reverse_bits_u8(byte);
    // printf("val %bx \n", tmp); // 测试反转后的数据

    // 根据命令，判断是从哪个寄存器读出的数据，在根据手册对应的格式做转换
    if (cmd == (AIP1302_SEC_REG_ADDR + 1) ||   /* 秒寄存器 */
        cmd == (AIP1302_MIN_REG_ADDR + 1) ||   /* 分钟寄存器 */
        cmd == (AIP1302_HOUR_REG_ADDR + 1) ||  /* 小时寄存器 */
        cmd == (AIP1302_DATE_REG_ADDR + 1) ||  /* 日期寄存器 */
        cmd == (AIP1302_MONTH_REG_ADDR + 1) || /* 月份寄存器 */
        cmd == (AIP1302_YEAR_REG_ADDR + 1))    /* 年寄存器 */
    {
        u8 tens_place = 0; // 十位
        u8 one_place = 0;  // 个位

        if ((AIP1302_SEC_REG_ADDR + 1) == cmd ||
            (AIP1302_MIN_REG_ADDR + 1) == cmd) {
            // 秒寄存器、分寄存器中中，清除bit7,bit6~bit0才是有效数据
            // 如果在时钟芯片上电复位后，没有开启晶振，这里会读取到 tmp==255
            tmp &= ~0x80;
        } else if ((AIP1302_HOUR_REG_ADDR + 1) == cmd ||
                   (AIP1302_DATE_REG_ADDR + 1) == cmd) {
            // 小时、日寄存器，清除bit7和bit6,未使用
            tmp &= ~0xC0;
        } else if ((AIP1302_MONTH_REG_ADDR + 1) == cmd) {
            // 月寄存器，bit7~5未使用
            tmp &= ~0xE0;
        }

        tens_place = tmp >> 4;
        one_place = tmp & (~0xF0);
        tmp = tens_place * 10 + one_place;
    }

    return tmp;
}

// 从aip1302中读取出原始的数据
static u8 __aip1302_read_byte__(const u8 cmd)
{
    u8 recv_data = 0; // 存放读取到的数据
    u8 i = 0;         // 循环计数值

    // DATA脚--配置为输出模式
    aip1302_data_mode_out();

    AIP1302_DATA_PIN = 0;
    AIP1302_SCLK_PIN = 0;
    AIP1302_CE_PIN = 1;
    AIP1302_DELAY(); // 等待电平信号稳定

    for (i = 0; i < 8; i++) {
        AIP1302_SCLK_PIN = 0;
        AIP1302_DELAY(); // 等待电平信号稳定
        if ((cmd >> i) & 0x01) {
            AIP1302_DATA_PIN = 1;
        } else {
            AIP1302_DATA_PIN = 0;
        }

        AIP1302_DELAY(); // 等待电平信号稳定
        AIP1302_SCLK_PIN = 1;
        AIP1302_DELAY(); // 等待电平信号稳定
    }

    // 将DATA脚切换为输入模式
    aip1302_data_mode_in();

    for (i = 0; i < 8; i++) {
        recv_data <<= 1;
        AIP1302_SCLK_PIN = 0;
        AIP1302_DELAY(); // 等待电平信号稳定
        if (AIP1302_DATA_PIN) {
            recv_data |= 0x01;
        } else {
        }
        AIP1302_SCLK_PIN = 1;
        AIP1302_DELAY(); // 等待电平信号稳定
    }

    AIP1302_SCLK_PIN = 0;
    AIP1302_CE_PIN = 0;
    AIP1302_DATA_PIN = 0;

    return recv_data;
}

/**
 * @brief aip1302写一个字节，函数内部的数据传输是LSB，低位先行
 *          函数内部会根据命令，对读出的数据做好转换
 *
 * @param cmd 读命令
 * @return u8 从aip1302内部读取出来的、转换好的数据
 */
u8 aip1302_read_byte(const u8 cmd)
{
    u8 recv_data = 0; // 存放读取到的数据
    recv_data = __aip1302_read_byte__(cmd);
    recv_data = __aip1302_convert_read_byte__(cmd, recv_data);
    return recv_data;
    // return __aip1302_convert_read_byte__(cmd, recv_data);
}

/**
 * @brief aip1302写一个字节，函数内部的数据传输是LSB，低位先行
 *
 * @param cmd 写命令
 * @param byte 待写入的一个字节的数据
 */
static void __aip1302_write_byte__(u8 cmd, u8 byte)
{
    u8 i = 0; // 循环计数值

    // DATA脚--配置为输出模式
    aip1302_data_mode_out();

    AIP1302_DATA_PIN = 0;
    AIP1302_SCLK_PIN = 0;
    AIP1302_CE_PIN = 1;
    AIP1302_DELAY(); // 等待电平信号稳定

    for (i = 0; i < 8; i++) {
        AIP1302_SCLK_PIN = 0;
        AIP1302_DELAY(); // 等待电平信号稳定
        if ((cmd >> i) & 0x01) {
            AIP1302_DATA_PIN = 1;
        } else {
            AIP1302_DATA_PIN = 0;
        }
        AIP1302_DELAY(); // 等待电平信号稳定
        AIP1302_SCLK_PIN = 1;
        AIP1302_DELAY(); // 等待电平信号稳定
    }

    for (i = 0; i < 8; i++) {
        AIP1302_SCLK_PIN = 0;
        AIP1302_DELAY(); // 等待电平信号稳定
        if ((byte >> i) & 0x01) {
            AIP1302_DATA_PIN = 1;
        } else {
            AIP1302_DATA_PIN = 0;
        }
        AIP1302_DELAY(); // 等待电平信号稳定
        AIP1302_SCLK_PIN = 1;
        AIP1302_DELAY(); // 等待电平信号稳定
    }

    AIP1302_SCLK_PIN = 0;
    AIP1302_DELAY(); // 等待电平信号稳定
    AIP1302_CE_PIN = 0;
    AIP1302_DATA_PIN = 0;
}

/**
 * @brief 判断aip1302的时钟晶振是否在运行
 *
 * @return
 *      u8 0--时钟晶振停止，1--时钟晶振运行
 *      u8 0--第一次上电，1--非第一次上电
 */
static u8 aip1302_is_working(void)
{
#if 0 // TEST ONLY
    u8 recv_data = __aip1302_read_byte__(AIP1302_SEC_REG_ADDR + 1); // 读取到的是反转后的数据
    // 秒寄存器的最高位Bit7是时钟停止标志位，1--时钟晶振停止，进入低功耗，0--晶振运转
    if (recv_data & 0x01) {
        // 如果时钟ic的时钟晶振停止
        return 0;
    } else {
        // 如果时钟ic的时钟晶振在运行
        return 1;
    }

#else

    // 判断时间是否正确：
    aip1302_read_all(); //
    if (aip1302_info.year > 2099 || aip1302_info.year < 2000 ||
        aip1302_info.month > 12 || aip1302_info.month < 1 ||
        aip1302_info.day > 31 || aip1302_info.day < 1 ||
        aip1302_info.time_hour >= 24 || aip1302_info.time_min >= 60 ||
        aip1302_info.time_sec >= 60 ||
        aip1302_info.year == 2000 /* 2000年，也认为是第一次上电 */
    ) {
        return 0; // 第一次上电
    } else {
        return 1; // 非第一次上电
    }

#endif
}

/**
 * @brief 向aip1302写入数据
 *          1. 内部会进行格式转换,以适配寄存器的格式
 *          会根据命令,屏蔽传入的数据的一些数据位
 *          2. 函数内部在写入前会解开芯片的写保护，写入完成后会打开芯片的写保护
 *
 * @param cmd 命令，待写入的寄存器的地址
 * @param byte 要写入的1字节数据，函数内部会对数据做好相应的处理，以符合对应的寄存器的格式
 *
 * @return  void
 */
void aip1302_write_byte(const u8 cmd, u8 byte)
{
    u8 tmp = byte;
    // 根据命令cmd，将对应的数据做好转换(转换成寄存器中存放的格式)，再写入

    if (cmd == AIP1302_SEC_REG_ADDR || cmd == AIP1302_MIN_REG_ADDR ||
        cmd == AIP1302_HOUR_REG_ADDR || cmd == AIP1302_DATE_REG_ADDR ||
        cmd == AIP1302_MONTH_REG_ADDR || cmd == AIP1302_YEAR_REG_ADDR) {
        u8 tens_place = 0; // 十位
        u8 one_place = 0;  // 个位
        if (tmp >= 10) {
            tens_place = tmp / 10;
        }
        one_place = tmp % 10;

        // 秒、分钟、小时、日、月、年寄存器都是高4位数据存放以10为单位的数据，低4位存放以1为单位的数据
        tmp = tens_place << 4 | one_place;
        // 秒寄存器中，清除bit7,bit7==0,时钟IC的晶振会继续振荡
        // 在分钟寄存器的bit7未使用，默认就是0
        // 年寄存器的bit7存放有效数据，不清零
        if (AIP1302_SEC_REG_ADDR == cmd || AIP1302_MIN_REG_ADDR == cmd) {
            tmp &= ~0x80;
        } else if (AIP1302_HOUR_REG_ADDR == cmd ||
                   AIP1302_DATE_REG_ADDR == cmd) {
            // 小时寄存器，清除bit7和bit6，bit7清零，表示使用24h制，bit6未使用
            // 日寄存器，bit7和bit6未使用
            tmp &= ~0xC0;
        } else if (AIP1302_MONTH_REG_ADDR == cmd) {
            // 月寄存器，bit7~5未使用
            tmp &= ~0xE0;
        }
    }

    // 解除写保护
    __aip1302_write_byte__(AIP1302_WRITE_PROTECT_REG_ADDR, 0x00);
    __aip1302_write_byte__(cmd, tmp); // 写入数据
    // 开启写保护
    __aip1302_write_byte__(AIP1302_WRITE_PROTECT_REG_ADDR, 0x80);
}

#if AIP1302_TEST_ENABLE //

static volatile u16 aip1302_test_cnt = 0;

void aip1302_test_1ms_isr(void)
{
    if (aip1302_test_cnt < ((u16)-1)) {
        aip1302_test_cnt++;
    }
}

void aip1302_test(void)
{
    u8 ret = 0;

    if (aip1302_test_cnt >= 2000) // 每隔一段时间，打印一次时钟ic中的时间
    {
        aip1302_test_cnt = 0;
        ret = aip1302_read_byte(AIP1302_YEAR_REG_ADDR + 1);
        printf("year %bu \n", ret);

        ret = aip1302_read_byte(AIP1302_MONTH_REG_ADDR + 1);
        printf("month %bu \n", ret);

        ret = aip1302_read_byte(AIP1302_DATE_REG_ADDR + 1);
        printf("day %bu \n", ret);

        // ret = aip1302_read_byte(AIP1302_WEEKDAY_REG_ADDR + 1);
        // printf("weekday %bu ", ret);

        ret = aip1302_read_byte(AIP1302_HOUR_REG_ADDR + 1);
        printf("hour %bu \n", ret);

        ret = aip1302_read_byte(AIP1302_MIN_REG_ADDR + 1);
        printf("min %bu \n", ret);

        ret = aip1302_read_byte(AIP1302_SEC_REG_ADDR + 1);
        printf("sec %bu ", ret);
        printf("\n");
    }
}
#endif

#if 0
// 向aip1302更新所有关于时间的数据
void aip1302_update_all_data(aip1302_info_t aip1302_info)
{
    aip1302_write_byte(AIP1302_SEC_REG_ADDR, aip1302_info.time_sec);
    aip1302_write_byte(AIP1302_MIN_REG_ADDR, aip1302_info.time_min);
    aip1302_write_byte(AIP1302_HOUR_REG_ADDR, aip1302_info.time_hour);
    aip1302_write_byte(AIP1302_DATE_REG_ADDR, aip1302_info.day);
    aip1302_write_byte(AIP1302_MONTH_REG_ADDR, aip1302_info.month);
    aip1302_write_byte(AIP1302_YEAR_REG_ADDR, aip1302_info.year - 2000); // 时钟IC内部只存0~99年
    // aip1302_write_byte(AIP1302_WEEKDAY_REG_ADDR, );
}
#endif

// 向aip1302更新 年、月、日、时、分、秒
void aip1302_update_time(aip1302_info_t aip1302_info)
{
    aip1302_write_byte(AIP1302_YEAR_REG_ADDR, (u16)aip1302_info.year - 2000);
    aip1302_write_byte(AIP1302_MONTH_REG_ADDR, aip1302_info.month);
    aip1302_write_byte(AIP1302_DATE_REG_ADDR, aip1302_info.day);

    aip1302_write_byte(AIP1302_HOUR_REG_ADDR, aip1302_info.time_hour);
    aip1302_write_byte(AIP1302_MIN_REG_ADDR, aip1302_info.time_min);
    aip1302_write_byte(AIP1302_SEC_REG_ADDR, aip1302_info.time_sec);
}

#if 0 
// 向aip1302更新日期,年月日
void aip1302_update_date(aip1302_info_t aip1302_info)
{
    aip1302_write_byte(AIP1302_DATE_REG_ADDR, aip1302_info.day);
    aip1302_write_byte(AIP1302_MONTH_REG_ADDR, aip1302_info.month);
    aip1302_write_byte(AIP1302_YEAR_REG_ADDR, aip1302_info.year - 2000);
}
#endif

// 向aip1302更新星期
// void aip1302_update_weekday(aip1302_info_t aip1302_info)
// {
//     aip1302_write_byte(AIP1302_WEEKDAY_REG_ADDR, aip1302_info.weekday);
// }

// 从aip1302中读出所有有关时间的数据,存放到全局变量中
void aip1302_read_all(void)
{
    aip1302_info.time_sec = aip1302_read_byte(AIP1302_SEC_REG_ADDR + 1);
    aip1302_info.time_min = aip1302_read_byte(AIP1302_MIN_REG_ADDR + 1);
    aip1302_info.time_hour = aip1302_read_byte(AIP1302_HOUR_REG_ADDR + 1);
    aip1302_info.day = aip1302_read_byte(AIP1302_DATE_REG_ADDR + 1);
    aip1302_info.month = aip1302_read_byte(AIP1302_MONTH_REG_ADDR + 1);
    aip1302_info.year =
        (u16)2000 + (u16)aip1302_read_byte(AIP1302_YEAR_REG_ADDR + 1);
    // aip1302_info.weekday = aip1302_read_byte(AIP1302_WEEKDAY_REG_ADDR + 1);
}

#if 0 // 测试程序

// 测试aip1302的写保护功能--测试通过
{
    static u8 time_cnt = 0;
    static u8 flag = 0;
    recv_data = aip1302_read_byte(AIP1302_YEAR_REG_ADDR + 1);
    printf("recv data: %bu\n", recv_data);
    time_cnt++;
    aip1302_write_byte(AIP1302_YEAR_REG_ADDR, time_cnt);

    if (flag == 0 && time_cnt >= 5)
    {
        time_cnt = 0;
        aip1302_write_byte(AIP1302_WRITE_PROTECT_REG_ADDR, 0x80); // 使能写保护
        flag = 1;
    }
    else if (flag == 1 && time_cnt >= 5)
    {
        time_cnt = 0;
        aip1302_write_byte(AIP1302_WRITE_PROTECT_REG_ADDR, 0x00); // 解除写保护
        flag = 0;
    }
}

// 测试aip1302是否开启了写保护--测试通过
{
    u8 ret = aip1302_read_byte(AIP1302_WRITE_PROTECT_REG_ADDR + 1);
    if (ret & 0x01)
    {
        // 如果芯片开启了写保护，解除写保护
        printf("write protect\n");
    }
    else
    {
        // 如果没有开启写保护
        printf("no write protect\n");
    }
}

#endif // 测试程序

#endif // IC_1302_ENABLE
