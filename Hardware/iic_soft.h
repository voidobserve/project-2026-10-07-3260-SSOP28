#ifndef __IIC_SOFT_H__
#define __IIC_SOFT_H__

#include "typedef.h"

// 目前只使用 1 条软件 IIC 总线

#define IIC_SOFT_SCL P22 /* iic 时钟引脚 SCL  */
#define IIC_SOFT_SDA P23 /* iic 数据引脚 SDA  */

#define IIC_SOFT_SCL_MODE_REG  P2_MD0
#define IIC_SOFT_SCL_MODE_MASK GPIO_P22_MODE_SEL(0x03)
#define IIC_SOFT_SCL_OUT_MASK  GPIO_P22_MODE_SEL(0x01)
#define IIC_SOFT_SCL_FOUT_REG  FOUT_S22 

#define IIC_SOFT_SDA_MODE_REG  P2_MD0
#define IIC_SOFT_SDA_MODE_MASK GPIO_P23_MODE_SEL(0x03)
#define IIC_SOFT_SDA_OUT_MASK  GPIO_P23_MODE_SEL(0x01)
#define IIC_SOFT_SDA_PULL_REG  P2_PU
#define IIC_SOFT_SDA_PULL_MASK GPIO_P23_PULL_UP(0x01)
#define IIC_SOFT_SDA_FOUT_REG  FOUT_S23

/* 总线延时，48MHz 主频下 delay(4) 约为几百 ns */
#define IIC_SOFT_DELAY() delay(5)

/* 7 位器件地址 + 读写位，组成完整的 8 位设备地址 */
#define IIC_SOFT_DEV_ADDR_W(dev) ((u8)(((u8)(dev) << 1) | 0x00)) /* 写 */
#define IIC_SOFT_DEV_ADDR_R(dev) ((u8)(((u8)(dev) << 1) | 0x01)) /* 读 */

// iic_soft 初始化
void iic_soft_config(void);

/**
 * @brief  从器件的指定寄存器地址开始读出指定长度的数据
 * @param  dev_addr       : 器件地址(7位，内部自动左移并补上读写位)
 * @param  reg_addr       : 寄存器/存储单元地址
 * @param  reg_addr_bytes : 寄存器地址字节数，1--单字节(如24C02)；2--双字节(如24C512)
 * @param  buf            : 读出数据的缓冲区首地址
 * @param  len            : 要读出的字节数
 * @retval 0--成功，1--失败
 */
u8 iic_soft_read(u8 dev_addr, u16 reg_addr, u8 reg_addr_bytes, u8 *buf,
                 u16 len);

/**
 * @brief  从器件的指定寄存器地址开始写入指定长度的数据
 * @param  dev_addr       : 器件地址(7位，内部自动左移并补上读写位)
 * @param  reg_addr       : 寄存器/存储单元地址
 * @param  reg_addr_bytes : 寄存器地址字节数，1--单字节(如24C02)；2--双字节(如24C512)
 * @param  buf            : 待写入数据的缓冲区首地址
 * @param  len            : 要写入的字节数
 * @retval 0--成功，1--失败
 */
u8 iic_soft_write(u8 dev_addr, u16 reg_addr, u8 reg_addr_bytes, const u8 *buf,
                  u16 len);

void iic_soft_start(void);
void iic_soft_write_byte(u8 dat);
u8 iic_soft_write_reg_addr(u16 reg_addr, u8 reg_addr_bytes);
u8 iic_soft_wait_ack(void);
u8 iic_soft_read_byte(u8 ack);
void iic_soft_stop(void);

/**
 * @brief  轮询等待器件就绪（等待器件内部的写周期结束）
 * @param  dev_addr   : 器件地址(7位，内部自动左移并补上读写位)
 * @param  timeout_ms : 等待超时时间(单位: ms)，由调用者按器件手册配置
 * @retval 0--器件已就绪(收到应答)，1--超时
 */
u8 iic_soft_wait_ready(u8 dev_addr, u16 timeout_ms);

#endif
