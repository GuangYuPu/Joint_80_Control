//############################################################
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//版权所有，盗版必究
//EtherCAT从站学习板
//Author：廷华电子设计
//淘宝店铺: https://shop461235811.taobao.com/
//############################################################

#include "stm32f4xx.h"
#include "ecat_def.h"

/*******************************************************************************
* Function Name  : TIM_Configuration
* Description    : TIM_Configuration program.
* Input          : None
* Output         : None
* Return         : None
* Attention		 : None
*******************************************************************************/
void TIM_Configuration(uint8_t period)		//100us
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);

    TIM_DeInit(TIM3);
    TIM_TimeBaseStructure.TIM_Period= period*100 ; //200;		 					/* 自动重装载寄存器周期的值(计数值) */
    /* 累计 TIM_Period个频率后产生一个更新或者中断 */
    TIM_TimeBaseStructure.TIM_Prescaler= (60 - 1);				    /* 时钟预分频数   例如：时钟频率=84MHZ/(时钟预分频+1) */
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 			/* 采样分频 */
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		/* 向上计数模式 */
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM3, TIM_FLAG_Update);							    /* 清除溢出中断标志 */
#if ECAT_TIMER_INT
    TIM_ITConfig(TIM3,TIM_IT_Update,ENABLE);
#endif
    TIM_Cmd(TIM3, ENABLE);											/* 开启时钟 */

#if ECAT_TIMER_INT
    /* Configure NVIC Interrupt  -------------------------*/
//    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//    NVIC_InitStructure.NVIC_IRQChannel = TIM3_IRQn;
//    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
//    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
//    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
//    NVIC_Init(&NVIC_InitStructure);
		NVIC_SetPriority(TIM3_IRQn,6);
		NVIC_EnableIRQ(TIM3_IRQn);
#endif

}
