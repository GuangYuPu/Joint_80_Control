
#include "MyProject.h"
#include "tamagawa.h"
#include "ABZ.h"
/*****************************************************************************/
void TIM1_PWM_Init(void)
{

    //	NVIC_InitTypeDef          NVIC_InitStructure;
    GPIO_InitTypeDef          GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef   TIM_TimeBaseInitStructure;
    TIM_OCInitTypeDef         TIM_OCInitStructure;
    TIM_BDTRInitTypeDef       TIM_BDTRInitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);

    GPIO_PinAFConfig(GPIOA,GPIO_PinSource8,GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource9,GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOA,GPIO_PinSource10,GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_TIM1);
    GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_TIM1);

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

    TIM_DeInit(TIM1);
    TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned1;
    TIM_TimeBaseInitStructure.TIM_Period =  SYS_FREQ/PWM_FREQ/2;    //168M/7000=24KHz
    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = FOCDiv;      //重复计数器，每(2+1)次
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStructure);

    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    //使能输出
    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;  //使能互补输出
    TIM_OCInitStructure.TIM_Pulse = 0;
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         //输出极性为高
    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;       //输出互补极性为高
    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;   //空闲状态为低
    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset; //空闲状态为低
    TIM_OC1Init(TIM1, &TIM_OCInitStructure);
    TIM_OC2Init(TIM1, &TIM_OCInitStructure);
    TIM_OC3Init(TIM1, &TIM_OCInitStructure);

    TIM_OC1PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC2PreloadConfig(TIM1, TIM_OCPreload_Enable);
    TIM_OC3PreloadConfig(TIM1, TIM_OCPreload_Enable);

    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
    TIM_BDTRInitStructure.TIM_DeadTime = 20;    // 100/168M=595ns
    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
    TIM_BDTRConfig(TIM1, &TIM_BDTRInitStructure);
    TIM_SelectOutputTrigger(TIM1, TIM_TRGOSource_Update);	  //更新作为触发输出
    TIM_ARRPreloadConfig(TIM1, ENABLE);

    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    //TIM_Cmd(TIM1, ENABLE);      //先不使能，在LowsideCurrentSense_Init()中使能









}
//void TIM8_PWM_Init(void)
//{


//    GPIO_InitTypeDef          GPIO_InitStructure;
//    TIM_TimeBaseInitTypeDef   TIM_TimeBaseInitStructure;
//    TIM_OCInitTypeDef         TIM_OCInitStructure;
//    TIM_BDTRInitTypeDef       TIM_BDTRInitStructure;

//    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA|RCC_AHB1Periph_GPIOB|RCC_AHB1Periph_GPIOC, ENABLE);
//    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

//    GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM8);
//    GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM8);
//    GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM8);
//    GPIO_PinAFConfig(GPIOA,GPIO_PinSource7,GPIO_AF_TIM8);
//    GPIO_PinAFConfig(GPIOB,GPIO_PinSource0,GPIO_AF_TIM8);
//    GPIO_PinAFConfig(GPIOB,GPIO_PinSource1,GPIO_AF_TIM8);

//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//    GPIO_Init(GPIOC, &GPIO_InitStructure);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);

//    TIM_DeInit(TIM8);
//    TIM_TimeBaseInitStructure.TIM_Prescaler = 0;
//    TIM_TimeBaseInitStructure.TIM_CounterMode = TIM_CounterMode_CenterAligned3;
//    TIM_TimeBaseInitStructure.TIM_Period =SYS_FREQ/PWM_FREQ/2;     //168M/7000=24KHz
//    TIM_TimeBaseInitStructure.TIM_ClockDivision = TIM_CKD_DIV1;
//    TIM_TimeBaseInitStructure.TIM_RepetitionCounter = FOCDiv;      //重复计数器，每(2+1)次产生一个溢出中断，所以中断频率是8KHz
//    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseInitStructure);

//    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
//    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;    //使能输出
//    TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Enable;  //使能互补输出
//    TIM_OCInitStructure.TIM_Pulse = 0;
//    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;         //输出极性为高
//    TIM_OCInitStructure.TIM_OCNPolarity = TIM_OCNPolarity_High;       //输出互补极性为高
//    TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;   //空闲状态为低
//    TIM_OCInitStructure.TIM_OCNIdleState = TIM_OCNIdleState_Reset; //空闲状态为低
//    TIM_OC1Init(TIM8, &TIM_OCInitStructure);
//    TIM_OC2Init(TIM8, &TIM_OCInitStructure);
//    TIM_OC3Init(TIM8, &TIM_OCInitStructure);

//    TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);
//    TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);
//    TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);

//    TIM_BDTRInitStructure.TIM_OSSRState = TIM_OSSRState_Enable;
//    TIM_BDTRInitStructure.TIM_OSSIState = TIM_OSSIState_Enable;
//    TIM_BDTRInitStructure.TIM_LOCKLevel = TIM_LOCKLevel_OFF;
//    TIM_BDTRInitStructure.TIM_DeadTime = 50;    // 100/168M=595ns
//    TIM_BDTRInitStructure.TIM_Break = TIM_Break_Disable;
//    TIM_BDTRInitStructure.TIM_BreakPolarity = TIM_BreakPolarity_High;
//    TIM_BDTRInitStructure.TIM_AutomaticOutput = TIM_AutomaticOutput_Disable;
//    TIM_BDTRConfig(TIM8, &TIM_BDTRInitStructure);

//    TIM_SelectOutputTrigger(TIM8, TIM_TRGOSource_Update);	  //更新作为触发输出
//    TIM_ARRPreloadConfig(TIM8, ENABLE);
//    TIM_CtrlPWMOutputs(TIM8, ENABLE);
//    //TIM_Cmd(TIM8, ENABLE);      //先不使能，在LowsideCurrentSense_Init()中使能
//}

/*****************************************************************************/
void TIM5_Init(void)
{
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);

    TIM_DeInit(TIM5);
    TIM_TimeBaseStructure.TIM_Period= 10-1 ; //200;		 					/* 自动重装载寄存器周期的值(计数值) */
    /* 累计 TIM_Period个频率后产生一个更新或者中断 */
    TIM_TimeBaseStructure.TIM_Prescaler= 8400-1;				    /* 时钟预分频数   例如：时钟频率=84MHZ/(时钟预分频+1) */
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 			/* 采样分频 */
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		/* 向上计数模式 */
    TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM5, TIM_FLAG_Update);							    /* 清除溢出中断标志 */

    TIM_ITConfig(TIM5,TIM_IT_Update,ENABLE);
    											/* 开启时钟 */

    /* Configure NVIC Interrupt  -------------------------*/

		NVIC_SetPriority (TIM5_IRQn, 5);
		NVIC_EnableIRQ(TIM5_IRQn);
}


float testfreq = 0;
float testdutycycle = 0;
void TIM4_Init(void)
{

    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);

    TIM_DeInit(TIM4);
    TIM_TimeBaseStructure.TIM_Period= 10-1 ; //200;		 					/* 自动重装载寄存器周期的值(计数值) */
    /* 累计 TIM_Period个频率后产生一个更新或者中断 */
    TIM_TimeBaseStructure.TIM_Prescaler= 8400-1;				    /* 时钟预分频数   例如：时钟频率=84MHZ/(时钟预分频+1) */
    TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 			/* 采样分频 */
    TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 		/* 向上计数模式 */
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseStructure);
    TIM_ClearFlag(TIM4, TIM_FLAG_Update);							    /* 清除溢出中断标志 */

    TIM_ITConfig(TIM4,TIM_IT_Update,ENABLE);
    											/* 开启时钟 */

    /* Configure NVIC Interrupt  -------------------------*/

		NVIC_SetPriority (TIM4_IRQn, 4);
		NVIC_EnableIRQ(TIM4_IRQn);
	
}



void IWDG_Init_Config(void)
{
	//取消写保护
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	//设置分频系统
	IWDG_SetPrescaler(IWDG_Prescaler_32); 
	//32分频  32khz/32=1khz  倒数一个数：1/1khz = 1/1000hz =0.001s = 1ms
	
	//设置重装载值
	IWDG_SetReload(2000);  //数2000个数，用时2s
	IWDG_ReloadCounter();
	
	//启动
	IWDG_Enable();
 
}