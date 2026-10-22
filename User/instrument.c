#include <string.h> // memset()
#include "instrument.h"

#include "eeprom_24cxx.h"
#include "user_config.h"

volatile instrument_t instrument = {0};
static volatile u8 is_intrument_save_enable = 0; // 是否允许保存数据到 flash
// 延时写入flash的时间计数
static volatile u16 instrument_info_save_time_cnt = 0;

static volatile save_info_t save_info = {0};

void instrument_info_init(void)
{
    u8 i;
    u32 max_total_mileage = 0;     // 当前获取到的最大总里程
    u32 max_sub_total_mileage = 0; // 当前获取到的最大短距离里程

    u32 total_mileage_inv;     // 存放当前获取到的总里程反码
    u32 sub_total_mileage_inv; // 存放当前获取到的短距离里程反码

    u8 reg_addr = 0;

    // 遍历eerom，读出校验通过且里程为最大的数据
    memset(&save_info, 0, sizeof(save_info_t));
    for (i = 0; i < EEPROM_USER_BLOCK_SIZE; i++) {
        // eeprom_24cxx_read();
    }
}

void instrument_info_save(void)
{
#if USE_INTERNAL_FLASH_SAVE_DATA

    instrument.save_info.is_save_data_valid =
        USER_FLASH_DATA_VALID_VAL; // 表示数据有效，让下一次上电读出数据时，验证该标志位
    // 先擦除扇区再写入
    flash_erase_sector(FLASH_START_ADDR);
    flash_program(FLASH_START_ADDR, (u8 *)&instrument.save_info,
                  sizeof(save_info_t));

#if USER_DEBUG_ENABLE
// printf("instrument info save\n");
#endif

#endif
}

// 使能延时写入flash的操作
void instrument_info_save_enable(void)
{
    instrument_info_save_time_cnt = 0;
    is_intrument_save_enable = 1;
}

// 递增延时写入flash的时间
void instrument_info_save_time_add(void)
{
    if (is_intrument_save_enable) {
        // if (instrument_info_save_time_cnt < ((u16)-1)) {
        instrument_info_save_time_cnt++;
        // }
    }
}

void instrument_info_save_handle(void)
{
    if (is_intrument_save_enable &&
        instrument_info_save_time_cnt >= INSTRUMENT_INFO_SAVE_TIME) {

        is_intrument_save_enable = 0;
        instrument_info_save_time_cnt = 0;
        instrument_info_save();
    }
}
