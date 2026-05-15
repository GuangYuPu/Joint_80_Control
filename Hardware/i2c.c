
#include "MyProject.h"
#include <stdbool.h>
#include "AS5600.h"
vu8 *I2C_ReadPtr;
vu8 *I2C_WritePtr;
vu32 I2CT_FLAG_TIMEOUTCnt = I2CT_FLAG_TIMEOUT;
DMA_InitTypeDef DMA_InitStruct;
void I2CX_init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;
    I2C_InitTypeDef I2C_InitStructure;

    NVIC_InitTypeDef NVIC_InitStruct;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    //      GPIO_DeInit(GPIOB);

    //        /* Configure pins as AF pushpull */
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2);

    // GPIO
    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;    // 推挽复用输出
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL; // 上拉
    GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    //		IICUnlock();

    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
    I2C_DeInit(I2C2);    // 复位I2C
    I2C1->CR1 |= 0x8000; // 手动清除清BUSY
    I2C1->CR1 &= ~0x8000;
    I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;    // I2C模式
    I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2; // 占空比(快速模式时)
    I2C_InitStructure.I2C_OwnAddress1         = 0;               // 设备地址
    I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;  // 应答
    I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
    I2C_InitStructure.I2C_ClockSpeed          = 1000000; // 速度
    I2C_Init(I2C2, &I2C_InitStructure);
    I2C_AcknowledgeConfig(I2C2, ENABLE);
    I2C_Cmd(I2C2, ENABLE);
}

uint8_t i2cbusy = 0;
u32 I2C_DMA_Read(u8 SlaveAddr, u8 ReadAddr, u8 *ReadBuf, u8 *NumByte)
{
    I2C_ReadPtr          = NumByte;
    i2cbusy              = I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY);
    I2CT_FLAG_TIMEOUTCnt = I2CT_LONG_TIMEOUT;
    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BUSY)) {
        if ((I2CT_FLAG_TIMEOUTCnt--) == 0) {
            IICUnlock();
            IICreStart();
            return 1;
        }
    }

    I2C_GenerateSTART(I2C2, ENABLE);

    I2CT_FLAG_TIMEOUTCnt = I2CT_FLAG_TIMEOUT;
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
        if ((I2CT_FLAG_TIMEOUTCnt--) == 0) return 1;

    I2C_Send7bitAddress(I2C2, SlaveAddr, I2C_Direction_Transmitter);

    I2CT_FLAG_TIMEOUTCnt = I2CT_FLAG_TIMEOUT;
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
        if ((I2CT_FLAG_TIMEOUTCnt--) == 0) return 1;

    I2C_SendData(I2C2, ReadAddr);

    I2CT_FLAG_TIMEOUTCnt = I2CT_FLAG_TIMEOUT;
    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_BTF) == RESET)
        if ((I2CT_FLAG_TIMEOUTCnt--) == 0) return 1;

    I2C_GenerateSTART(I2C2, ENABLE);

    I2CT_FLAG_TIMEOUTCnt = I2CT_FLAG_TIMEOUT;
    while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
        if ((I2CT_FLAG_TIMEOUTCnt--) == 0) return 1;

    I2C_Send7bitAddress(I2C2, SlaveAddr, I2C_Direction_Receiver);

    //	if((u16)(*NumByte) < 2) {
    I2CT_FLAG_TIMEOUTCnt = I2CT_FLAG_TIMEOUT;
    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_ADDR) == RESET)
        if ((I2CT_FLAG_TIMEOUTCnt--) == 0) return 1;

    I2C_AcknowledgeConfig(I2C2, ENABLE);
    (void)I2C2->SR2;

    I2C_GenerateSTOP(I2C2, ENABLE);

    I2CT_FLAG_TIMEOUTCnt = I2CT_FLAG_TIMEOUT;
    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_RXNE) == RESET)
        if ((I2CT_FLAG_TIMEOUTCnt--) == 0) return 1;

    *ReadBuf = I2C_ReceiveData(I2C2);
    ReadBuf++;
    I2C_AcknowledgeConfig(I2C2, DISABLE);
    while (I2C_GetFlagStatus(I2C2, I2C_FLAG_RXNE) == RESET)
        if ((I2CT_FLAG_TIMEOUTCnt--) == 0) return 1;
    *ReadBuf = I2C_ReceiveData(I2C2);

    //		I2C_GenerateSTOP(I2C2, ENABLE);
    I2CT_FLAG_TIMEOUTCnt = I2CT_FLAG_TIMEOUT;
    while (I2C2->CR1 & I2C_CR1_STOP)
        if ((I2CT_FLAG_TIMEOUTCnt--) == 0) return 1;

    I2C_AcknowledgeConfig(I2C2, ENABLE);

    return SUCCESS;
}
uint8_t testIICSDA  = 0;
uint8_t testIIC2SDA = 0;
void IICUnlock(void)
{

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽复用输出
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;  // 上拉
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; // 推挽复用输出
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;  // 上拉
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    GPIO_SetBits(GPIOB, GPIO_Pin_10);
    //	GPIO_SetBits(GPIOB,GPIO_Pin_11);
    testIICSDA = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11);
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 0) {
        testIICSDA = 1;
        for (uint8_t i = 0; i < 9; i++) {

            GPIO_ResetBits(GPIOB, GPIO_Pin_10);
            delay_us(2);
            GPIO_SetBits(GPIOB, GPIO_Pin_10);
            delay_us(2);
        }
    }
}

void IICreStart(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    if (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11) == 1) {
        testIIC2SDA = 1;
        //					GPIO_DeInit(GPIOB);

        /* Configure pins as AF pushpull */
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource10, GPIO_AF_I2C2);
        GPIO_PinAFConfig(GPIOB, GPIO_PinSource11, GPIO_AF_I2C2);

        // GPIO
        GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_10 | GPIO_Pin_11;
        GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
        GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;    // 推挽复用输出
        GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL; // 上拉
        GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
        GPIO_Init(GPIOB, &GPIO_InitStructure);

        I2C_InitTypeDef I2C_InitStructure;
        RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
        I2C_DeInit(I2C2);    // 复位I2C
        I2C1->CR1 |= 0x8000; // 手动清除清BUSY
        I2C1->CR1 &= ~0x8000;
        I2C_InitStructure.I2C_Mode                = I2C_Mode_I2C;    // I2C模式
        I2C_InitStructure.I2C_DutyCycle           = I2C_DutyCycle_2; // 占空比(快速模式时)
        I2C_InitStructure.I2C_OwnAddress1         = 0;               // 设备地址
        I2C_InitStructure.I2C_Ack                 = I2C_Ack_Enable;  // 应答
        I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
        I2C_InitStructure.I2C_ClockSpeed          = 1000000; // 速度
        I2C_Init(I2C2, &I2C_InitStructure);
        I2C_AcknowledgeConfig(I2C2, ENABLE);
        I2C_Cmd(I2C2, ENABLE);

        //				testIICSDA = 0;
    }
}
