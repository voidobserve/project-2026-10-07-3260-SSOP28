#include "adc.h"
#include "fuel_capacity.h"

// 控制切换adc通道的状态机：
static volatile u8 adc_channel_status = ADC_CHANNEL_STATUS_NONE;

// adc配置，使用adc时还需要切换到对应的引脚通道
void adc_config(void)
{
#if FUEL_CAPACITY_SCAN_ENABLE
    // 检测油量的引脚：
    P2_MD0 |= GPIO_P20_MODE_SEL(0x03); // 模拟模式
#endif

    ADC_CFG1 |= (0x0F << 3) | // ADC时钟分频为16分频，为系统时钟/16
                (0x01 << 0);  // adc0中断使能
    ADC_CFG2 = 0xFF;          // 通道0采样时间配置为256个采样时钟周期

    ADC_ACON0 = ADC_CMP_EN(0x1) |  // 打开ADC中的CMP使能信号
                ADC_BIAS_EN(0x1) | // 打开ADC偏置电流能使信号
                ADC_BIAS_SEL(0x1); // 偏置电流选择：1x

    // 通道 0DLY 的 ADC 时钟个数选择，配置为 4n+1，4 * 29 + 1
    ADC_TRGS0 |= (0x07 << 4);
    ADC_CHS0 |= (0x01 << 6); // 使能 通道 0DLY 功能
    __EnableIRQ(ADC_IRQn);   // 使能ADC中断
    IE_EA = 1;               // 使能总中断

    // 当前工程只有检测油量一个通道，直接固定配置该通道，不再切换
    adc_channel_set(ADC_CHANNEL_FUEL);
}

// 设置adc通道
void adc_channel_set(adc_channel_t adc_channel)
{
    // 清空选择的adc0通道
    ADC_CHS0 &=
        ~((0x01 << 4) | (0x01 << 3) | (0x01 << 2) | (0x01 << 1) | (0x01 << 0));
    ADC_ACON1 &= ~((0x01 << 6) | // 关闭ADC内部参考使能信号，不使能内部参考电压
                   (0x01 << 5) | // 关闭ADC外部参考使能信号，不使能外部参考电压
                   (0x07 << 0)); // 清空ADC内部参考电压的选择配置

    switch (adc_channel) {
#if FUEL_CAPACITY_SCAN_ENABLE
        // 检测油量
    case ADC_CHANNEL_FUEL:
        ADC_ACON1 |= (0x01 << 6) |         // 使能ADC内部参考信号
                     (0x03 << 3) |         // 关闭测试信号
                     (0x03 << 0);          // 内部参考电压选择 3.0 V
        ADC_CHS0 |= ADC_ANALOG_CHAN(0x10); // P20 通道
        break;
#endif
    }

    ADC_CFG0 |= ADC_CHAN0_EN(0x1) | // 使能通道0转换
                ADC_EN(0x1);        // 使能A/D转换
    // 切换完通道后，需要等待ADC模块配置稳定，至少20us以上
}

// 由定时器调用，adc通道切换
void adc_channel_switch_by_isr(void)
{
    switch (adc_channel_status) {
        // none 或 fuel_begin，开启adc0转换
    case ADC_CHANNEL_STATUS_NONE:
    case ADC_CHANNEL_STATUS_SEL_FUEL_BEGIN:
        ADC_CFG0 |= ADC_CHAN0_TRG(0x1); // 触发ADC0转换
        adc_channel_status = ADC_CHANNEL_STATUS_SEL_FUEL_END;
        break;
    case ADC_CHANNEL_STATUS_SEL_FUEL_END:
        adc_channel_status = ADC_CHANNEL_STATUS_SEL_FUEL_BEGIN;
        break;
    }
}

void ADC_IRQHandler(void) interrupt ADC_IRQn
{
    volatile u16 adc_val; // 由后续赋值

    // 进入中断设置IP，不可删除
    __IRQnIPnPush(ADC_IRQn);

    // ---------------- 用户函数处理 -------------------
    if (ADC_STA & ADC_CHAN0_DONE(0x01)) {
        ADC_STA |= ADC_CHAN0_DONE(0x01); // 清除ADC0转换完成标志位
        adc_val = (ADC_DATAH0 << 4) | (ADC_DATAL0 >> 4); // 读取ADC0的值

        switch (adc_channel_status) {
#if FUEL_CAPACITY_SCAN_ENABLE
        case ADC_CHANNEL_STATUS_SEL_FUEL_END:
            fuel_capacity_adc_val_samples_update(adc_val);
            break;
#endif
        }
    }

    // 退出中断设置IP，不可删除
    __IRQnIPnPop(ADC_IRQn);
}
