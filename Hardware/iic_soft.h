#ifndef __IIC_SOFT_H__
#define __IIC_SOFT_H__

#include "typedef.h"

// 目前只使用 1 条软件 IIC 总线
// 端口的初始化在 iic_soft.c 中实现，本头文件只提供对外接口

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

#endif
