#include "uart0.h"
#include "include.h" // 使用芯片官方提供的头文件

// 设置的波特率需要适配单片机的时钟，这里直接使用了官方的代码
#define USER_UART0_BAUD ((SYSCLK - UART0_BAUDRATE) / (UART0_BAUDRATE))

#if 1 // 将uart0用作串口打印
// 重写putchar()函数
extern void uart0_send_byte(u8 byte); // 函数声明
char putchar(char c)
{
    uart0_send_byte(c);
    return c;
}
#endif // 将uart0用作串口打印

void uart0_init(void)
{
    // UART0 TX
    P2_MD1 &= ~(GPIO_P25_MODE_SEL(0x03)); // 清空寄存器配置
    P2_MD1 |= GPIO_P25_MODE_SEL(0x01);    // 输出模式
    FOUT_S25 |= GPIO_FOUT_UART0_TX;       // 配置为 UART0_TX

    UART0_BAUD1 = (USER_UART0_BAUD >> 8) & 0xFF; // 配置波特率高八位
    UART0_BAUD0 = USER_UART0_BAUD & 0xFF;        // 配置波特率低八位
    UART0_CON0 = UART_STOP_BIT(0x0) |            // 1bit停止位
                 UART_EN(0x1);                   // 使能 uart

    __EnableIRQ(UART0_IRQn); // 打开UART模块中断
    IE_EA = 1;               // 打开总中断
}

void uart0_send_byte(u8 byte)
{
    // 等待之前的数据发送完成
    while (!(UART0_STA & UART_TX_DONE(0x01)))
        ;
    UART0_DATA = byte;
}

void UART0_IRQHandler(void) interrupt UART0_IRQn
{
    u8 uart_data = 0;
    // 进入中断设置IP，不可删除
    __IRQnIPnPush(UART0_IRQn);

    // ---------------- 用户函数处理 -------------------

    // 退出中断设置IP，不可删除
    __IRQnIPnPop(UART0_IRQn);
}
