#include "aip3368_driver.h"
#include "include.h"
#include <string.h>      // memset
#include "user_config.h" // USER_DEBUG_ENABLE

// REVIEW 测试时使用 u16 , 实际使用可以改为 u8
// static volatile u8 aip3368h_refresh_cnt = 0;
static volatile u16 aip3368h_refresh_cnt = 0;

// 显存
volatile u16 aip3368h_display_buff[AIP3368H_IC_NUM] = {0};

// 放在1ms的定时器中
void aip3368h_refresh_time_add(void)
{
    // 防止计数溢出
    if (aip3368h_refresh_cnt < ((u16)-1)) {
        aip3368h_refresh_cnt++;
    }
}

static void aip3368h_module_send_data(const u16 *buf, const u8 len)
{
    volatile u8 i;
    volatile u8 j;
    volatile u16 dat;

    // 开始
    DCK = 0;
    LAT = 0;
    aip3368h_delay();

    // 一帧完整数据
    for (i = 0; i < len; i++) {
        dat = buf[i];
        for (j = 0; j < 16; j++) {
            DIO = dat & (u16)0x8000 ? 1 : 0;

            aip3368h_delay();
            DCK = 1;
            aip3368h_delay();

            dat <<= 1;
            DCK = 0;
            aip3368h_delay();
        }
    }

    // 结束
    LAT = 1;
    aip3368h_delay();
    LAT = 0;
    aip3368h_delay();
    DIO = 0;
}

#define AIP3368H_FLASH_TEST_ENABLE 0

// 根据显存中的数据，更新显示
void aip3368h_module_display(void)
{
    // 刷新间隔 单位：ms
    // if (aip3368h_refresh_cnt < 25) {
    if (aip3368h_refresh_cnt < 500) {
        return;
    } else {
        aip3368h_refresh_cnt = 0;
    }

#if USER_DEBUG_ENABLE
// printf("aip3368h_module_display\n");
#endif

#if AIP3368H_FLASH_TEST_ENABLE

    // printf("aip3368h_speed_panel_display_buff[0] == 0x%04x\n",
    //        aip3368h_speed_panel_display_buff[0]);

    // 闪烁测试
    if (aip3368h_display_buff[0] == (u16)0x0000) {
        memset(aip3368h_display_buff, 0xFF, sizeof(aip3368h_display_buff));
    } else {
        memset(aip3368h_display_buff, 0x00, sizeof(aip3368h_display_buff));
    }

#endif

    aip3368h_module_send_data(aip3368h_display_buff, AIP3368H_IC_NUM);
}

void aip3368h_module_init(void)
{
    // 显示驱动芯片有记忆功能（数据锁存），每次上电应该清空显存
    memset(aip3368h_display_buff, 0x00, sizeof(aip3368h_display_buff));

    // DCK
    P1_MD1 &= ~GPIO_P14_MODE_SEL(0x03);
    P1_MD1 |= GPIO_P14_MODE_SEL(0x01); // 输出
    FOUT_S14 = GPIO_FOUT_AF_FUNC;
    // DIO
    P1_MD0 &= ~GPIO_P13_MODE_SEL(0x03);
    P1_MD0 |= GPIO_P13_MODE_SEL(0x01); // 输出
    FOUT_S13 = GPIO_FOUT_AF_FUNC;
    // LAT
    P3_MD0 &= ~GPIO_P30_MODE_SEL(0x03);
    P3_MD0 |= GPIO_P30_MODE_SEL(0x01); // 输出
    FOUT_S30 = GPIO_FOUT_AF_FUNC;

    DIO = 0;
    DCK = 0;
    LAT = 0;

    // 显示驱动芯片带有记忆功能，上电之后需要先写入一次全为0的数据，再写入实际数据
    aip3368h_module_send_data(aip3368h_display_buff, AIP3368H_IC_NUM);
}
