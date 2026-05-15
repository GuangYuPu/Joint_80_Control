#include "MCU_Com.h"

// 定义全局变量
TPV_DATA_T_SPI TPV_DATA_SPI = {0};
MotionControlType_SPI Motor_Control_SPI = Type_angle_SPI;
MotorStatusType_SPI Motor_status_SPI = PosZero_SPI;
TPV_DATA_T_1TO2_SPI TPV_data_2to1_SPI = {0};
TPV_DATA_T_1TO2_SPI TPV_data_1to2_SPI = {0};

// 计算数据结构大小（以16位为单位）
#define DATA_SIZE_2TO1 (sizeof(TPV_DATA_T_2TO1_SPI) / sizeof(uint16_t))
#define DATA_SIZE_1TO2 (sizeof(TPV_DATA_T_1TO2_SPI) / sizeof(uint16_t))
#define MAX_DATA_SIZE (DATA_SIZE_2TO1 > DATA_SIZE_1TO2 ? DATA_SIZE_2TO1 : DATA_SIZE_1TO2)

// 内部使用的发送和接收缓冲区
static uint16_t mcu1_tx_buffer[MAX_DATA_SIZE]; // MCU1发送缓冲区（发送给MCU2的数据）
static uint16_t mcu1_rx_buffer[MAX_DATA_SIZE]; // MCU1接收缓冲区（从MCU2接收的数据）

static uint16_t mcu2_tx_buffer[MAX_DATA_SIZE]; // MCU2发送缓冲区（发送给MCU1的数据）
static uint16_t mcu2_rx_buffer[MAX_DATA_SIZE]; // MCU2接收缓冲区（从MCU1接收的数据）

/**
 * @brief  MCU1的SPI初始化配置
 * @param  None
 * @retval None
 */
void User_Communication_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    // 1. 使能相关时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    // 2. 配置GPIO引脚
    // PB9 作为SPI1_CS引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // PB3, PB4, PB5 作为SPI1的SCK, MISO, MOSI引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置引脚复用功能
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);

    // 3. 配置SPI1为主模式
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b; // 16位数据
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    // 4. 使能SPI1
    SPI_Cmd(SPI1, ENABLE);

    // 5. 设置CS引脚为高电平（空闲状态）
    GPIO_SetBits(GPIOB, GPIO_Pin_9);
}

/**
 * @brief  MCU1的SPI数据传输函数
 * @param  pTxData: 发送数据指针
 * @param  pRxData: 接收数据指针
 * @param  Size: 数据大小
 * @retval None
 */
static void MCU1_SPI_TransmitReceive(uint16_t *pTxData, uint16_t *pRxData, uint16_t Size)
{
    uint16_t i = 0;

    // 拉低CS引脚，开始传输
    GPIO_ResetBits(GPIOB, GPIO_Pin_9);

    for (i = 0; i < Size; i++)
    {
        // 等待发送缓冲区为空
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
            ;

        // 发送数据
        SPI_I2S_SendData(SPI1, pTxData[i]);

        // 等待接收缓冲区非空
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
            ;

        // 接收数据
        pRxData[i] = SPI_I2S_ReceiveData(SPI1);
    }

    // 等待传输完成
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
        ;

    // 拉高CS引脚，结束传输
    GPIO_SetBits(GPIOB, GPIO_Pin_9);
}

/**
 * @brief  MCU1的循环执行发送和接收函数
 * @param  None
 * @retval None
 */
void User_Communication_Loop(void)
{
    uint16_t *pData1to2 = (uint16_t *)&TPV_DATA_SPI.TPV_data_1to2_SPI;
    TPV_DATA_T_2TO1_SPI *pRxData = &TPV_DATA_SPI.TPV_data_2to1_SPI;
    uint16_t *pRxBuffer = (uint16_t *)pRxData;

    // 1. 准备要发送给MCU2的数据（MCU1 -> MCU2）
    for (uint16_t i = 0; i < DATA_SIZE_1TO2; i++)
    {
        mcu1_tx_buffer[i] = pData1to2[i];
    }

    // 填充缓冲区剩余部分为0
    for (uint16_t i = DATA_SIZE_1TO2; i < MAX_DATA_SIZE; i++)
    {
        mcu1_tx_buffer[i] = 0;
    }

    // 2. 执行SPI数据传输
    MCU1_SPI_TransmitReceive(mcu1_tx_buffer, mcu1_rx_buffer, MAX_DATA_SIZE);

    // 3. 处理接收到的数据（MCU2 -> MCU1）
    for (uint16_t i = 0; i < DATA_SIZE_2TO1; i++)
    {
        pRxBuffer[i] = mcu1_rx_buffer[i];
    }
}

/**
 * @brief  MCU2的SPI初始化配置
 * @param  None
 * @retval None
 */
void User_Communication_Init2(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    SPI_InitTypeDef SPI_InitStructure;

    // 1. 使能相关时钟
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

    // 2. 配置GPIO引脚
    // PB6 作为SPI1_CS引脚（输入模式，用于检测片选）
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // PB3, PB4, PB5 作为SPI1的SCK, MISO, MOSI引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    // 配置引脚复用功能
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);

    // 3. 配置SPI1为从模式
    SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
    SPI_InitStructure.SPI_Mode = SPI_Mode_Slave;
    SPI_InitStructure.SPI_DataSize = SPI_DataSize_16b; // 16位数据
    SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
    SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
    SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
    SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
    SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
    SPI_InitStructure.SPI_CRCPolynomial = 7;
    SPI_Init(SPI1, &SPI_InitStructure);

    // 4. 使能SPI1
    SPI_Cmd(SPI1, ENABLE);
}

/**
 * @brief  MCU2的SPI数据传输函数
 * @param  pTxData: 发送数据指针
 * @param  pRxData: 接收数据指针
 * @param  Size: 数据大小
 * @retval None
 */
static void MCU2_SPI_TransmitReceive(uint16_t *pTxData, uint16_t *pRxData, uint16_t Size)
{
    uint16_t i = 0;

    // 等待CS引脚被拉低（主设备开始通信）
    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == SET)
        ;

    for (i = 0; i < Size; i++)
    {
        // 等待发送缓冲区为空
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET)
            ;

        // 发送数据
        SPI_I2S_SendData(SPI1, pTxData[i]);

        // 等待接收缓冲区非空
        while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET)
            ;

        // 接收数据
        pRxData[i] = SPI_I2S_ReceiveData(SPI1);
    }

    // 等待传输完成
    while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_BSY) == SET)
        ;

    // 等待CS引脚被拉高（主设备结束通信）
    while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_6) == RESET)
        ;
}

/**
 * @brief  MCU2的循环执行发送和接收函数
 * @param  None
 * @retval None
 */
void User_Communication_Loop2(void)
{
    uint16_t *pData2to1 = (uint16_t *)&TPV_DATA_SPI.TPV_data_2to1_SPI;
    TPV_DATA_T_1TO2_SPI *pRxData = &TPV_DATA_SPI.TPV_data_1to2_SPI;
    uint16_t *pRxBuffer = (uint16_t *)pRxData;

    // 1. 准备要发送给MCU1的数据（MCU2 -> MCU1）
    for (uint16_t i = 0; i < DATA_SIZE_2TO1; i++)
    {
        mcu2_tx_buffer[i] = pData2to1[i];
    }

    // 填充缓冲区剩余部分为0
    for (uint16_t i = DATA_SIZE_2TO1; i < MAX_DATA_SIZE; i++)
    {
        mcu2_tx_buffer[i] = 0;
    }

    // 2. 执行SPI数据传输
    MCU2_SPI_TransmitReceive(mcu2_tx_buffer, mcu2_rx_buffer, MAX_DATA_SIZE);

    // 3. 处理接收到的数据（MCU1 -> MCU2）
    for (uint16_t i = 0; i < DATA_SIZE_1TO2; i++)
    {
        pRxBuffer[i] = mcu2_rx_buffer[i];
    }
}
