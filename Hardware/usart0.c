
#include <stdio.h>
#include "usart0.h"
#include "stdarg.h"
/********************************************************************/
uint8_t snd0_buff[USART0_BUFFER_SIZE0] = {0};
uint8_t rcv0_buff[USART0_BUFFER_SIZE0] = {0};
unsigned long rcv0_cntr;
unsigned long rcv0_flag;
/******************************************************************************/
//加入以下代码，支持printf函数，而不需要选择MicroLIB
#if 1
#pragma import(__use_no_semihosting)
//标准库需要的支持函数
struct __FILE
{
    int handle;
};

FILE __stdout;
//定义_sys_exit避免使用半主机模式
void _sys_exit(int x)
{
    x = x;
}
//重定义fputc函数
int fputc(int ch, FILE *f)
{
    while((USART1->SR&0x40)==0);
    USART1->DR = (u8) ch;
    return ch;
}
#endif
/********************************************************************/
void USART0_Init(unsigned long bound)
{

    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef  DMA_InitStructure;

    //RCC_Config
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_DMA2,ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

    //GPIO_AF_Map
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource6,GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource7,GPIO_AF_USART1);

    //GPIO
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6| GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;   //推挽复用输出
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;  //上拉
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOB,&GPIO_InitStructure);



    //USART2
    USART_DeInit(USART1);
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_Init(USART1,&USART_InitStructure);
		
		
		USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);
//		USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);
//    USART_ITConfig(USART1,USART_IT_IDLE,ENABLE);   //接收空闲中断
//		USART_ClearFlag(USART1, USART_FLAG_IDLE);  

		NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 7;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
		NVIC_SetPriority (USART1_IRQn, 7);
		NVIC_EnableIRQ(USART1_IRQn);
				
		USART_Cmd(USART1,ENABLE);
		
    DMA_DeInit(DMA2_Stream7);
    DMA_InitStructure.DMA_Channel = DMA_Channel_4;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (u32)&USART1->DR;
    DMA_InitStructure.DMA_Memory0BaseAddr = (u32)&snd0_buff;
    DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToPeripheral;
    DMA_InitStructure.DMA_BufferSize = USART0_BUFFER_SIZE0;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
    DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
    DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
    DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream7, &DMA_InitStructure);
    DMA_Cmd(DMA2_Stream7,ENABLE);
		
		NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream7_IRQn;     //DMA发送数据流中断通道
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   //抢占优先级
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;          //子优先级
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //IRQ通道使能
    NVIC_Init(&NVIC_InitStructure);
		NVIC_SetPriority (DMA2_Stream7_IRQn, 8);
		NVIC_EnableIRQ(DMA2_Stream7_IRQn);
		
		
		DMA_ITConfig(DMA2_Stream7,DMA_IT_TC,ENABLE);
		
		
		
//	DMA_InitStructure.DMA_Memory0BaseAddr     = (uint32_t) rcv0_buff; // 存储要发送的数据的地址
//	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(USART1->DR);	 // 向串口寄存器搬运数据
//	DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;
//	DMA_InitStructure.DMA_Channel = DMA_Channel_4;
//	DMA_InitStructure.DMA_BufferSize         = USART0_BUFFER_SIZE0;								 // 内存大小
//	DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;			 // 内存地址自增
//	DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;  // 外设地址禁止自增
//	DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_Byte;
//	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;
//  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
//	DMA_InitStructure.DMA_Mode               = DMA_Mode_Normal;						 // 设成正常模式，不要设成周期模式
//	DMA_InitStructure.DMA_Priority           = DMA_Priority_Low;
//  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_Single;
//  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
//	
//	DMA_Init( DMA2_Stream2, &DMA_InitStructure );												 // USART1->TX对应DMA1通道4
//	
//	DMA_Cmd(DMA2_Stream2,ENABLE);
//		
//		NVIC_InitStructure.NVIC_IRQChannel = DMA2_Stream2_IRQn;     //DMA发送数据流中断通道
////    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;   //抢占优先级
////    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;          //子优先级
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;             //IRQ通道使能
//    NVIC_Init(&NVIC_InitStructure);
//		NVIC_SetPriority (DMA2_Stream2_IRQn, 7);
//		NVIC_EnableIRQ(DMA2_Stream2_IRQn);
//		
//		
//		DMA_ITConfig(DMA2_Stream2,DMA_IT_TC,ENABLE);
		
}




void printf_DMA(char *fmt, ...)
{
		DMA_Cmd(DMA2_Stream7 , DISABLE);	// 先失能DMA通道4
		va_list args;	// 记录输入的参数
   uint16_t length; // 用于记录字符串长度

	va_start(args, fmt); // 对字符串进行转换
	length = vsnprintf((char*)snd0_buff, sizeof(snd0_buff) + 1, (char*)fmt, args); // 开始对DMA_Send_Buffer赋值，并返回字符串长度
	va_end(args);

	if(length > USART0_BUFFER_SIZE0-1) length = USART0_BUFFER_SIZE0-1; // 防止超出数组界限
	DMA_SetCurrDataCounter(DMA2_Stream7, length); // 设置要发送数据的长度
	DMA_Cmd(DMA2_Stream7 , ENABLE);	// 使能DMA通道4，开始搬运数据
	while(USART_GetFlagStatus(USART1,USART_FLAG_TC)==RESET);	//判断是否发送完成
}

//void USART1_IRQHandler(void)
//{
//	if(USART_GetITStatus(USART1,USART_IT_IDLE)!=RESET)
//	{
//		uint32_t temp;
//		rcv0_flag = 1;
//		uint8_t clear = clear;
//		
//		USART_ClearFlag(USART1, USART_FLAG_IDLE);   
//		clear = USART1->SR;
//		clear = USART1->DR;
//		DMA_Cmd(DMA2_Stream2,DISABLE);
//                                                                       
////    temp = USART0_BUFFER_SIZE0 - DMA_GetCurrDataCounter(DMA2_Stream2);
//		temp = DMA_GetCurrDataCounter(DMA2_Stream2);
//		DMA_ClearFlag(DMA2_Stream2,DMA_FLAG_TCIF2 | DMA_FLAG_FEIF2 | DMA_FLAG_DMEIF2 | DMA_FLAG_TEIF2 | DMA_FLAG_HTIF2);//清除DMA2_Steam5传输完成标志  
//		DMA_SetCurrDataCounter(DMA2_Stream2, temp);
//    DMA_Cmd(DMA2_Stream2, ENABLE);
////		while(USART_GetFlagStatus(USART1,USART_FLAG_RXNE)==SET);	//判断是否发送完成
//	}

//// if(USART_GetFlagStatus(USART1,USART_IT_TXE) == RESET)                                                                                //串口发送完成
////    {
////        USART_ITConfig(USART1,USART_IT_TC,DISABLE);
////    }

//}
/********************************************************************/
//void USART1_SendDMA(uint32_t len)
//{
//    DMA_Cmd(DMA1_Stream6,DISABLE);                         //关闭DMA传输
//    //while(DMA_GetCmdStatus(DMA1_Stream6) != DISABLE){}     //确保DMA可以被设置
//    DMA_SetCurrDataCounter(DMA1_Stream6, len);             //设置传输字节数
//    DMA_Cmd(DMA1_Stream4,ENABLE);
//    USART_DMACmd(USART1, USART_DMAReq_Tx, ENABLE);         //开启DMA传输
//}
/********************************************************************
 * USART0 接收空闲中断服务函数
********************************************************************/
//void USART1_IRQHandler(void)
//{
//    if(USART_GetITStatus(USART1,USART_IT_IDLE) == SET)
//    {
//        rcv0_cntr=USART1->SR;
//        rcv0_cntr=USART1->DR;     //清除中断标志USART_ClearITPendingBit(USART2,USART_IT_RXNE)
//        rcv0_cntr=USART0_BUFFER_SIZE0-DMA_GetCurrDataCounter(DMA1_Stream5);  //读取接收字节个数
//        DMA_Cmd(DMA1_Stream5,DISABLE);                           //关闭DMA传输
//        while(DMA_GetCmdStatus(DMA1_Stream5) != DISABLE) {}      //确保DMA可以被设置
//        DMA_SetCurrDataCounter(DMA1_Stream5,USART0_BUFFER_SIZE0); //数据传输量
//        DMA_Cmd(DMA1_Stream5,ENABLE);                            //开启DMA传输
//        rcv0_flag=1;
//    }
//}
/********************************************************************
 * USART0 DMA发送中断服务函数
********************************************************************/
void DMA2_Stream7_IRQHandler  (void)
{
    if(DMA_GetITStatus(DMA2_Stream7,DMA_IT_TCIF7) == SET)
    {
        DMA_ClearITPendingBit(DMA2_Stream7,DMA_IT_TCIF7);
        DMA_Cmd(DMA2_Stream7,DISABLE);   //本次发送完成，关闭DMA，下次使用再打开
    }
}

//void DMA2_Stream2_IRQHandler  (void)
//{
//    if(DMA_GetITStatus(DMA2_Stream2,DMA_IT_TCIF2) == SET)
//    {
//        DMA_ClearITPendingBit(DMA2_Stream2,DMA_IT_TCIF2);
//        DMA_Cmd(DMA2_Stream2,DISABLE);   //本次发送完成，关闭DMA，下次使用再打开
//    }
//}
/********************************************************************
void USART0_one(uint8_t da)
{
	USART_SendData(USART0,da);
	while(USART_GetFlagStatus(USART0,USART_FLAG_TXE)==RESET);	//判断是否发送完成
}
***************************************/
/*
void USART0_send(char *p,uint32_t num)
{
	while(num--)
	{
		USART_SendData(USART0,*p++);
		while(USART_GetFlagStatus(USART0,USART_FLAG_TXE)==RESET);	//判断是否发送完成
	}
}
*/
/********************************************************************/

