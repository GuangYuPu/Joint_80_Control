
#ifndef _I2C_H
#define _I2C_H

#include "stm32f4xx.h"
//#include <stdio.h>
#include <stdbool.h>
#define I2C_TIME		((u32)65535)
#define I2C0_SPEED             1000000
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x500)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))
static __IO uint32_t  I2CTimeout = I2CT_LONG_TIMEOUT;
/******************************************************************************/
void I2CX_init(void);
void i2c_bus_reset(void);
bool i2cWrite(uint32_t i2c_x, uint8_t slaveAddr, uint8_t addrbit, uint16_t addr, uint8_t *pdata, uint16_t len);
uint8_t IIC_read_len(uint8_t addr, uint8_t reg,uint8_t *buf,uint8_t len);
extern  void delay_s(uint32_t i);
/******************************************************************************/
u32 I2C_DMA_Read(u8 SlaveAddr,u8 ReadAddr ,u8* ReadBuf , u8* NumByte );
u32 I2C_DMA_ReadReg(  u8 SlaveAddr, u8 ReadAddr,u8* ReadBuf, u8 NumByte );
u32 I2C_DMA_Write( u8* WriteBuf, u8 SlaveAddr, u8 WriteAddr, u8* NumByte );
u32 I2C_DMA_WriteReg( u8* WriteBuf, u8 SlaveAddr, u8 WriteAddr, u8 NumByte );
void I2C1_Send_DMA_IRQ( void );
void I2C1_Recv_DMA_IRQ( void );
void IICUnlock(void);
void IICreStart(void);
u32 I2C_TimeOut( void );

#endif
