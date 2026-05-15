//############################################################
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//版权所有，盗版必究
//EtherCAT从站学习板
//Author：廷华电子设计
//淘宝店铺: https://shop461235811.taobao.com/
//############################################################

#include "stm32f4xx.h"
/*******************************************************************************
* Function Name  :EXTI0_Configuration
* Description    : EXTI0_Configuration初始化
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void EXTI2_Configuration(void) //SPI_IRQ PC13 --->PD0
{

    EXTI_InitTypeDef   EXTI_InitStructure;
    GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;

    /* Enable GPIOC clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Configure PA2 pin as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd =GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* Connect EXTI Line2 to PA2 pin */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource0);

    /* Configure EXTI Line2 */
    EXTI_InitStructure.EXTI_Line = EXTI_Line0;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set EXTI Line2 Interrupt to the lowest priority */
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//    NVIC_InitStructure.NVIC_IRQChannel =EXTI0_IRQn;// EXTI9_5_IRQ;//EXTI3_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
		
		NVIC_SetPriority(EXTI0_IRQn,6);
		NVIC_EnableIRQ(EXTI0_IRQn);
}
/*******************************************************************************
* Function Name  : EXTI1_Configuration
* Description    : EXTI1_Configuration 初始化
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void EXTI0_Configuration(void) //SYNC0--PC0
{
    EXTI_InitTypeDef   EXTI_InitStructure;
    GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;

    /* Enable GPIOA clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Configure PA0 pin as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* Connect EXTI Line0 to PA0 pin */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource1);

    /* Configure EXTI Line0 */
    EXTI_InitStructure.EXTI_Line = EXTI_Line1;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set EXTI Line0 Interrupt to the lowest priority */
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
		NVIC_SetPriority(EXTI0_IRQn,6);
		NVIC_EnableIRQ(EXTI0_IRQn);

}

/*******************************************************************************
* Function Name  : EXTI3_Configuration
* Description    : EXTI3_Configuration
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void EXTI3_Configuration(void)  //SYNC1:PA3
{
    EXTI_InitTypeDef   EXTI_InitStructure;
    GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;

    /* Enable GPIOA clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);

    /* Configure PA0 pin as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_Init(GPIOD, &GPIO_InitStructure);

    /* Connect EXTI Line0 to PA0 pin */
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource2);

    /* Configure EXTI Line0 */
    EXTI_InitStructure.EXTI_Line = EXTI_Line2;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set EXTI Line0 Interrupt to the lowest priority */
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//    NVIC_InitStructure.NVIC_IRQChannel = EXTI0_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
		NVIC_SetPriority(EXTI0_IRQn,6);
		NVIC_EnableIRQ(EXTI0_IRQn);
}
