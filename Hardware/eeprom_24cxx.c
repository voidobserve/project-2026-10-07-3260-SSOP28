#include "eeprom_24cxx.h"
#include "iic_soft.h"
#include "user_config.h"

#if USER_DEBUG_ENABLE
#include <stdio.h>
#endif

void eeprom_24cxx_config(void)
{
    iic_soft_config();
}

/**
 * @brief  向 24Cxx 写入一段数据，写完等待器件内部写周期结束
 *         注意：24Cxx 单次写入不能跨页(24CXX 页大小 xx 字节)，
 *               跨页时芯片会在页内回卷，需调用者自行保证不跨页
 * @param  addr : 存储单元地址
 * @param  buf  : 待写入数据的缓冲区首地址
 * @param  len  : 要写入的字节数
 */
void eeprom_24cxx_write(u8 addr, u8 *buf, u16 len)
{
    if (0 == iic_soft_write(EEPROM_DEV_ADDR, (u16)addr, 1, buf, len)) {
        // 写入成功，等待 24CXX 内部写周期结束
        iic_soft_wait_ready(EEPROM_DEV_ADDR, EEPROM_WAIT_READY_TIMEOUT_MS);
    }
}

#if EEPROM_24CXX_TEST_ENABLE
volatile u8 flag_is_test_eeprom_24cxx_period_coming = 0;
void test_eeprom_24cxx_period_1ms_isr(void)
{
    static volatile u16 cnt = 0;
    cnt++;
    if (cnt >= 2000) {
        cnt = 0;
        flag_is_test_eeprom_24cxx_period_coming = 1;
    }
}

void test_eeprom_24cxx(void)
{
    static volatile u8 buf[1] = {0};
    static volatile u8 val = 0;
    u8 ret = 0;

    if (0 == flag_is_test_eeprom_24cxx_period_coming) {
        return;
    }
    flag_is_test_eeprom_24cxx_period_coming = 0;

    buf[0] = val;
    val++;
    if (val >= 255) {
        val = 0;
    }
    ret = iic_soft_write(EEPROM_DEV_ADDR, 0x00, 1, buf, 1);
    if (ret) {
        printf("write fail\n");
        printf("err code == %02u\n", (u16)ret);
    }

    // 写完必须等 24CXX 内部写周期结束，否则紧接着的读会被 NACK
    if (iic_soft_wait_ready(EEPROM_DEV_ADDR, EEPROM_WAIT_READY_TIMEOUT_MS)) {
        printf("wait ready timeout\n");
    }

    buf[0] = 0;
    ret = iic_soft_read(EEPROM_DEV_ADDR, 0x00, 1, buf, 1);
    if (ret) {
        printf("read fail\n");
        printf("err code == %02u\n", (u16)ret);
    }
    printf("buf == %02x\n", (u16)buf[0]);
}

/**
 * @brief 测试 24Cxx 的器件地址
 *        在iic总线上依次写入器件地址(写)，看是否有应答
 * 
 * @return * void 
 */
void test_eeprom_24cxx_addr(void)
{
    u8 ret = 0;
    u8 i = 0;
    for (i = 0; i < 8; i++) {
        ret = 0;
        iic_soft_start();

        // 发送器件地址(写)
        iic_soft_write_byte(IIC_SOFT_DEV_ADDR_W(EEPROM_DEV_ADDR + i));
        if (0 == iic_soft_wait_ack()) {
            ret = 1;
        }

        iic_soft_stop();

        if (ret) {
            printf("addr == %02u ack\n", (u16)i);
        }
    }
}

#endif