#include "Protection.h"
#include "CurrentSense.h"
#include "tamagawa.h"

#define CurrentProtWindow PWM_FREQ/1000   //1ms检测

uint8_t EroType = 0;
uint16_t OverCurrentCount = 0;
uint16_t CurrentProtWindowCount = 0;
void ACCurrentProt(MOTORController* M,PhaseCurrent_s*I0)
{
	CurrentProtWindowCount++;
	
	
	if ( fabs(I0->a) > M->IProt|| fabs(I0->b) > M->IProt|| fabs(I0->c) > M->IProt)
	{
		OverCurrentCount++;
	}
	
	if(CurrentProtWindowCount == CurrentProtWindow)      //每个采样窗口进行一次判断
	{
		if(OverCurrentCount > CurrentProtWindow/2)
		{
			MosShut();
			motor_status = ERROR;
			EroType = PeakCurOver;
		}
		else
		{
			CurrentProtWindowCount = 0;
			OverCurrentCount = 0;
		}
	
	}
}

void CurrentOverLoadProt(void)
{
	static uint16_t CurMeanOvrCount = 0;

//	static uint32_t timeCount = 0;
//	static uint8_t TIMEnableFlag = 0,TIMDisableFlag = 0;

//	if(tempIsum > 2500) //50A
//	{
//		TIMDisableFlag = 0;
//		if(TIMEnableFlag == 0)
//		{
//			TIM_Cmd(TIM6,ENABLE);
//			TIMEnableFlag = 1;
//		}
//		
//		if(TIM6->CNT > 10000)  //20K计数频率，也就是5s
//		{
//			TIM6->CNT = 0;
//			TIM_Cmd(TIM6,DISABLE);
//			TIMDisableFlag = 1;
//			PWMShut();
//			motor_status = ERROR;
//			EroType = MeanCurOver;
//		}
		
//	}
//	else
//	{
//		TIMEnableFlag = 0;
//		if(TIMDisableFlag == 0)
//		{
//			TIM6->CNT = 0;
//			TIM_Cmd(TIM6,DISABLE);
//			TIMDisableFlag = 1;
//		}
//		
//	}
	static float tempIsum = 0;
	tempIsum = currentM0.q *currentM0.q + currentM0.d *currentM0.d;
	if(tempIsum > 2500) //50A
	{
		CurMeanOvrCount++;
	}
	else
	{
		CurMeanOvrCount = 0;
	}
	
	if(CurMeanOvrCount > 5000)
	{
			MosShut();
			motor_status = ERROR;
			EroType = MeanCurOver;
	}

}

void StuckProt(void)
{
	static uint16_t PosEroCount = 0;

	
	if(fabs(M0.P_ang.ref - position)>10)
	{
			PosEroCount++;
	}
	else
	{
		PosEroCount = 0;
	}
	if(PosEroCount>10000)  // 10s没有达到指定位置
	{
//			PWMShut();
//			motor_status = ERROR;
			EroType = PosStuck;
	}
}

void HeatProt(void)
{
	if(StatorTemp > 150)
	{
			MosShut();
			motor_status = ERROR;
			EroType = StatorOvrHeat;
	}
	if(BoardTemp > 95)
	{
			MosShut();
			motor_status = ERROR;
			EroType = BoardOvrHeat;
	}
}

void BusVolProt(void)
{
	if(voltage_power_supply >110)
	{
		EroType = BusVolOver;
	}
	
	if(voltage_power_supply < 60)
	{
		EroType = BusVolUnder;
	}
}


void PWMShut(void)
{
    TIM1 ->CCR1 = 0;
    TIM1 ->CCR2 = 0;
		TIM1 ->CCR3 = 0;

//    TIM8 ->CCR1 = 0;
//    TIM8 ->CCR2 = 0;
//		TIM8 ->CCR3 = 0;
			
    TIM_Cmd(TIM1, DISABLE);
//		TIM_Cmd(TIM8, DISABLE);
		motor_status = ERROR;
		PWMENflag = 1;

}

void PWMStandby(void)
{
    TIM1 ->CCR1 = 0;
    TIM1 ->CCR2 = 0;
		TIM1 ->CCR3 = 0;

//    TIM8 ->CCR1 = 0;
//    TIM8 ->CCR2 = 0;
//		TIM8 ->CCR3 = 0;
}

uint8_t PWMENflag = 0;

void PWMDisable(void)
{
    TIM1 ->CCR1 = 0;
    TIM1 ->CCR2 = 0;
		TIM1 ->CCR3 = 0;

//    TIM8 ->CCR1 = 0;
//    TIM8 ->CCR2 = 0;
//		TIM8 ->CCR3 = 0;
			
    TIM_Cmd(TIM1, DISABLE);
//		TIM_Cmd(TIM8, DISABLE);
	PWMENflag = 0;
}

void PWMEnable(void)
{
    TIM_Cmd(TIM1, ENABLE);
//		TIM_Cmd(TIM8, ENABLE);
		PWMENflag = 1;
}

uint8_t MosShutFlag = 0;
void MosShut(void)
{

	  GPIO_InitTypeDef          GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	

//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//    GPIO_Init(GPIOC, &GPIO_InitStructure);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//    GPIO_Init(GPIOA, &GPIO_InitStructure);
//    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
//    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
//    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
//    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
//    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
//    GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		
		
		GPIO_ResetBits(GPIOA,GPIO_Pin_8 | GPIO_Pin_9 | GPIO_Pin_10);
		GPIO_ResetBits(GPIOB,GPIO_Pin_13| GPIO_Pin_14| GPIO_Pin_15);
		
//		GPIO_ResetBits(GPIOC,GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8);
//		GPIO_ResetBits(GPIOA,GPIO_Pin_7);
//		GPIO_ResetBits(GPIOB,GPIO_Pin_0 | GPIO_Pin_1);
		
		MosShutFlag = 1;
		TimGpioReFlag = 0;
		motor_status = Standby;
}


uint8_t TimGpioReFlag = 0;
void TimGpioReInit(void)
{
	  GPIO_InitTypeDef          GPIO_InitStructure;
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
	

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
		
		TimGpioReFlag = 1;
		MosShutFlag = 0;
}

