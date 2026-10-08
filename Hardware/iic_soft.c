#include "iic_soft.h"
#include "include.h"

/* SDA配置为输入模式 */
static void iic_soft_sda_in(void)
{
    IIC_SOFT_SDA_MODE_REG &= ~IIC_SOFT_SDA_MODE_MASK; // 输入
    IIC_SOFT_SDA_PULL_REG |= IIC_SOFT_SDA_PULL_MASK;  // 上拉
}

static void iic_soft_sda_out(void)
{
    IIC_SOFT_SDA_PULL_REG &= ~IIC_SOFT_SDA_PULL_MASK; // 取消上拉
    IIC_SOFT_SDA_MODE_REG &= ~IIC_SOFT_SDA_MODE_MASK;
    IIC_SOFT_SDA_MODE_REG |= IIC_SOFT_SDA_OUT_MASK; // 输出
    IIC_SOFT_SDA_FOUT_REG = GPIO_FOUT_AF_FUNC;
}

/**
 * @brief  软件 IIC 端口初始化
 * @param  None
 * @retval None
 */
void iic_soft_config(void)
{
    // SCL
    IIC_SOFT_SCL_MODE_REG &= ~IIC_SOFT_SCL_MODE_MASK;
    IIC_SOFT_SCL_MODE_REG |= IIC_SOFT_SCL_OUT_MASK;
    IIC_SOFT_SCL_FOUT_REG = GPIO_FOUT_AF_FUNC; // 选择AF功能输出

    // SDA
    IIC_SOFT_SDA_MODE_REG &= ~IIC_SOFT_SDA_MODE_MASK;
    IIC_SOFT_SDA_MODE_REG |= IIC_SOFT_SDA_OUT_MASK;
    IIC_SOFT_SDA_FOUT_REG = GPIO_FOUT_AF_FUNC; // 选择AF功能输出

    IIC_SOFT_SDA = 1; // 空闲时数据线为高电平
    IIC_SOFT_SCL = 1; // 空闲时时钟线为高电平
}

/**
 * @brief  iic soft start function
 * @param  None
 * @retval None
 */
void iic_soft_start(void)
{
    // 起始时，系统时钟线（SCLK）和系统数据线（SDATA）都处于高电平
    iic_soft_sda_out(); // SDA线输出
    IIC_SOFT_SDA = 1;
    IIC_SOFT_SCL = 1;
    // 延时函数是为了等待配置完成，当SCLK为高电平时，SDATA跳变为低电平
    // 紧接着SCLK也跳变为低电平
    // 产生一个起始信号
    IIC_SOFT_DELAY();
    IIC_SOFT_SDA = 0; // START:when CLK is high,DATA change form high to low
    IIC_SOFT_DELAY();
    IIC_SOFT_SCL = 0;
}

/**
 * @brief  iic soft stop function
 * @param  None
 * @retval None
 */
void iic_soft_stop(void)
{
    iic_soft_sda_out(); // SDA线输出

    // 配置时钟线和数据线均为低电平
    IIC_SOFT_SCL = 0;
    IIC_SOFT_SDA = 0; // STOP:when CLK is high DATA change form low to high
    IIC_SOFT_DELAY();

    // 配置时钟线为高电平
    IIC_SOFT_SCL = 1;
    IIC_SOFT_DELAY();

    // 在时钟线为高电平时，输出的数据线从低电平跳变到高电平，则产生了停止信号
    IIC_SOFT_SDA = 1;
    IIC_SOFT_DELAY();
}

/**
 * @brief  iic soft wait ack function
 * @param  None
 * @retval 0:收到应答  1:超时无应答
 */
u8 iic_soft_wait_ack(void)
{
    u8 timeout = 0;

    iic_soft_sda_in(); // SDA设置为输入

    // 配置为高电平
    IIC_SOFT_SDA = 1;
    IIC_SOFT_DELAY();

    // 配置为高电平
    IIC_SOFT_SCL = 1;
    IIC_SOFT_DELAY();

    while (IIC_SOFT_SDA) {
        // 等待SDA拉低，表示应答带来，不然一直while循环，直到超时
        timeout++;
        if (timeout > 250) {
            IIC_SOFT_SCL = 0; // SCL拉低，停止信号由调用者统一发出
            return 1;         // 返回1，表示失败
        }
        IIC_SOFT_DELAY();
    }
    IIC_SOFT_SCL = 0; // SCL拉低

    return 0; // 返回0，表示成功
}

/**
 * @brief  iic soft ack function
 * @param  None
 * @retval None
 */
static void iic_soft_ack(void)
{
    IIC_SOFT_SCL = 0;
    iic_soft_sda_out();
    IIC_SOFT_SDA = 0;
    IIC_SOFT_DELAY();
    IIC_SOFT_SCL = 1;
    IIC_SOFT_DELAY();
    IIC_SOFT_SCL = 0;
}

/**
 * @brief  iic soft no ack function
 * @param  None
 * @retval None
 */
static void iic_soft_nack(void)
{
    IIC_SOFT_SCL = 0;
    iic_soft_sda_out();
    IIC_SOFT_SDA = 1;
    IIC_SOFT_DELAY();
    IIC_SOFT_SCL = 1;
    IIC_SOFT_DELAY();
    IIC_SOFT_SCL = 0;
}

/**
 * @brief  iic soft write 1 byte data function
 * @param  dat : 待写入的字节
 * @retval None
 */
void iic_soft_write_byte(u8 dat)
{
    u8 i;

    iic_soft_sda_out(); // SDA线输出模式

    // 只有在时钟线为0的情况下，数据线才可以进行高低电平的跳变
    IIC_SOFT_SCL = 0; // 拉低时钟开始数据传输

    for (i = 0; i < 8; i++) { // for循环，一位一位的发送，从最高位 位7开始
        IIC_SOFT_SDA =
            (dat & 0x80) >>
            7;     // 除了位7外，其余全屏蔽为0，然后右移到位0，给SDA数据线
        dat <<= 1; // 左移一位，准备下一次发送
        IIC_SOFT_DELAY();

        // 发送完成之后，时钟线拉高
        IIC_SOFT_SCL = 1;
        IIC_SOFT_DELAY();

        // 紧接着拉低
        IIC_SOFT_SCL = 0;
        IIC_SOFT_DELAY();

        // 发送完一个数据都将时钟线进行一次的拉高和拉低，完成一个位的传输
    }
}

/**
 * @brief  iic soft read 1 byte function
 * @param  ack : 1--读完后发送ACK；0--读完后发送NACK
 * @retval 读到的1个字节
 */
u8 iic_soft_read_byte(u8 ack)
{
    u8 i;
    u8 receive = 0;

    iic_soft_sda_in(); // SDA设置为输入

    for (i = 0; i < 8; i++) { // for循环，一位一位的读取，从最高位 位7开始
        IIC_SOFT_SCL = 0;
        IIC_SOFT_DELAY();
        IIC_SOFT_SCL = 1;
        receive <<= 1; // 左移一位，准备下次的读取

        if (IIC_SOFT_SDA) {
            receive++;
        }
        IIC_SOFT_DELAY();
    }

    if (!ack) {          // 不需要应答
        iic_soft_nack(); // 发送nACK
    } else {             // 需要应答
        iic_soft_ack();  // 发送ACK
    }

    return receive;
}

/**
 * @brief  发送寄存器地址
 * @param  reg_addr       : 寄存器/存储单元地址
 * @param  reg_addr_bytes : 寄存器地址字节数，1--单字节；2--双字节
 * @retval 0--成功，1--失败
 */
u8 iic_soft_write_reg_addr(u16 reg_addr, u8 reg_addr_bytes)
{
    if (reg_addr_bytes >= 2) {
        iic_soft_write_byte((u8)(reg_addr >> 8)); // 发送高地址
        if (iic_soft_wait_ack()) {
            return 1;
        }
    }

    iic_soft_write_byte((u8)(reg_addr & 0xFF)); // 发送低地址
    if (iic_soft_wait_ack()) {
        return 1;
    }

    return 0;
}

/**
 * @brief  从器件的指定寄存器地址开始读出指定长度的数据
 * @param  dev_addr       : 器件地址(7位，内部自动左移并补上读写位)
 * @param  reg_addr       : 寄存器/存储单元地址
 * @param  reg_addr_bytes : 寄存器地址字节数，1--单字节(如24C02)；2--双字节(如24C512)
 * @param  buf            : 读出数据的缓冲区首地址
 * @param  len            : 要读出的字节数
 * @retval 0--成功，1--失败
 */
u8 iic_soft_read(u8 dev_addr, u16 reg_addr, u8 reg_addr_bytes, u8 *buf, u16 len)
{
    u8 ret = 0;
    u16 i;

    // 入参校验：缓冲区/长度非0，寄存器地址字节数只能为1或2
    if ((buf == 0) || (len == 0) ||
        ((reg_addr_bytes != 1) && (reg_addr_bytes != 2))) {
        ret = 1; // 入参非法
        goto iic_soft_read_err;
    }

    // 起始信号
    iic_soft_start();

    // 发送器件地址(写)
    iic_soft_write_byte(IIC_SOFT_DEV_ADDR_W(dev_addr));
    if (iic_soft_wait_ack()) {
        ret = 2;
        goto iic_soft_read_err;
    }

    // 发送寄存器地址
    if (iic_soft_write_reg_addr(reg_addr, reg_addr_bytes)) {
        ret = 3;
        goto iic_soft_read_err;
    }

    // 重新起始，进入读模式
    iic_soft_start();
    iic_soft_write_byte(IIC_SOFT_DEV_ADDR_R(dev_addr));
    if (iic_soft_wait_ack()) {
        ret = 4;
        goto iic_soft_read_err;
    }

    // 连续读出数据，最后一个字节回 NACK
    for (i = 0; i < len; i++) {
        buf[i] = iic_soft_read_byte((i == (len - 1)) ? 0 : 1);
    }

iic_soft_read_err:
    iic_soft_stop(); // 出错也要释放总线
    return ret;
}

/**
 * @brief  从器件的指定寄存器地址开始写入指定长度的数据
 * @param  dev_addr       : 器件地址(7位，内部自动左移并补上读写位)
 * @param  reg_addr       : 寄存器/存储单元地址
 * @param  reg_addr_bytes : 寄存器地址字节数，1--单字节(如24C02)；2--双字节(如24C512)
 * @param  buf            : 待写入数据的缓冲区首地址
 * @param  len            : 要写入的字节数
 * @retval 0--成功，非0--对应的错误码
 */
u8 iic_soft_write(u8 dev_addr, u16 reg_addr, u8 reg_addr_bytes, const u8 *buf,
                  u16 len)
{
    u8 ret = 0;
    u16 i;

    // 入参校验：缓冲区/长度非0，寄存器地址字节数只能为1或2
    if ((buf == 0) || (len == 0) ||
        ((reg_addr_bytes != 1) && (reg_addr_bytes != 2))) {
        ret = 1; // 入参非法
        goto iic_soft_write_err;
    }

    // 起始信号
    iic_soft_start();

    // 发送器件地址(写)
    iic_soft_write_byte(IIC_SOFT_DEV_ADDR_W(dev_addr));
    if (iic_soft_wait_ack()) {
        ret = 2; // 发送器件地址失败
        goto iic_soft_write_err;
    }

    // 发送寄存器地址
    if (iic_soft_write_reg_addr(reg_addr, reg_addr_bytes)) {
        ret = 3; // 发送寄存器地址失败
        goto iic_soft_write_err;
    }

    // 依次写入数据
    for (i = 0; i < len; i++) {
        iic_soft_write_byte(buf[i]);
        if (iic_soft_wait_ack()) {
            ret = 4; // 写入数据失败
            goto iic_soft_write_err;
        }
    }

iic_soft_write_err:
    iic_soft_stop(); // 出错也要释放总线
    return ret;
}

/**
 * @brief  轮询等待器件就绪（等待器件内部的写周期结束）
 *         对 24Cxx 等 EEPROM，写入后芯片内部需要最多 5ms 完成擦写，
 *         此期间器件不响应任何命令，必须等它重新应答后才能继续访问
 * @param  dev_addr   : 器件地址(7位，内部自动左移并补上写位)
 * @param  timeout_ms : 等待超时时间(单位: ms)，由调用者按器件手册配置
 * @retval 0--器件已就绪(收到应答)，1--超时(器件一直不响应)
 */
u8 iic_soft_wait_ready(u8 dev_addr, u16 timeout_ms)
{
    u16 cnt = 0;

    do {
        iic_soft_start();
        iic_soft_write_byte(IIC_SOFT_DEV_ADDR_W(dev_addr));

        // 收到应答，说明器件的写周期已结束，可以继续访问
        if (0 == iic_soft_wait_ack()) {
            iic_soft_stop();
            return 0;
        }
        iic_soft_stop(); // 未应答，作废本次访问，释放总线后重试

        delay_ms(1); // 每次重试间隔 1ms（delay_ms 内部会喂狗）
        cnt++;
    } while (cnt < timeout_ms);

    return 1; // 超时：器件一直不响应
}
