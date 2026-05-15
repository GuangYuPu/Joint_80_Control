#include "ABZ.h"
#include "main.h"
#include "stdlib.h"
#include "Protection.h"
#include "timer.h"
#include "adc.h"
#include "BLDCMotor.h"
#include "OrthPLL.h"
uint8_t PosiRotA[3];
uint8_t PosiRotB[3];
uint8_t NegtRotA[3];
uint8_t NegtRotB[3];

ATOCALIB_T ATOCALIB;

void EXTIB_Configuration(void)
{
    EXTI_InitTypeDef   EXTI_InitStructure;
    GPIO_InitTypeDef   GPIO_InitStructure;
    NVIC_InitTypeDef   NVIC_InitStructure;

    /* Enable GPIOC clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
		RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);
    /* Enable SYSCFG clock */
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);


    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd =GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
		GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
    GPIO_Init(GPIOB, &GPIO_InitStructure);

		/*A线外部中断*/
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource10);


    EXTI_InitStructure.EXTI_Line = EXTI_Line10;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);


		/*B线外部中断*/
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOB, EXTI_PinSource11);


    EXTI_InitStructure.EXTI_Line = EXTI_Line11;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
		
		/*Z脉冲外部中断*/
    SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource15);


    EXTI_InitStructure.EXTI_Line = EXTI_Line15;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising;
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
		
		NVIC_SetPriority(EXTI15_10_IRQn,0);
		NVIC_EnableIRQ(EXTI15_10_IRQn);
		
		PosiRotA[0] = 0;
		PosiRotA[1] = 1;
		PosiRotA[2] = 1;
		
		PosiRotB[0] = 1;
		PosiRotB[1] = 1;
		PosiRotB[2] = 0;
		
		NegtRotA[0] = 1;
		NegtRotA[1] = 1;
		NegtRotA[2] = 0;
		
		NegtRotB[0] = 0;
		NegtRotB[1] = 1;
		NegtRotB[2] = 1;
}
uint8_t Astatus[3];
uint8_t Bstatus[3];
uint32_t CptrA = 0;
uint32_t CptrB = 0;
uint32_t AB_Count = 0;
int8_t direction;
int16_t Diff = 0;
uint16_t testcount = 0;
ABZEncoder_T ABZEncoder;
void EXTI15_10_IRQHandler(void)
{
	/*进入外部触发中断，判断状态*/
	if(EXTI_GetFlagStatus(EXTI_Line10)!=RESET)
	{
		
		Bstatus[0] = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_11);
			/*判断计数值是否+1*/
		if(Bstatus[0] != Bstatus[1])
		{
			ABZEncoder.AB_count_new++;
			CptrA++;
		}
		EXTI_ClearITPendingBit(EXTI_Line10);
	}
	

	if(EXTI_GetFlagStatus(EXTI_Line11)!=RESET)
	{
		Astatus[0] = GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_10);
		/*判断计数值是否+1*/
		if(Astatus[0] != Astatus[1])
		{
			ABZEncoder.AB_count_new++;
			CptrB++;
		}
		EXTI_ClearITPendingBit(EXTI_Line11);
	}
	/*判断正反转*/
	if(ArrayEqual(Astatus,Bstatus,PosiRotA,PosiRotB,3))
	{
		ABZEncoder.direction = -1;
	}
	if(ArrayEqual(Astatus,Bstatus,NegtRotA,NegtRotB,3))
	{
		ABZEncoder.direction = +1;
	}
	/*数组移相，将[0]位数据作为每次接收的最新数据*/
	Astatus[2] = Astatus[1];
	Astatus[1] = Astatus[0];
	
	Bstatus[2] = Bstatus[1];
	Bstatus[1] = Bstatus[0];
	
	
	/*外部Z脉冲中断处理，清楚计数值*/
	if(EXTI_GetFlagStatus(EXTI_Line15)!=RESET)
	{
//		testcount = ABZEncoder.AB_count_new;

//		Diff = ABZEncoder.AB_count_new - testcount;
//		testcount = ABZEncoder.AB_count_new;
//		PWMShut();
//		AngleAdd = 0;
		ABZEncoder.AB_count_new = 0;
		ABZEncoder.ANGLE_ROTOR_INT = 42010;
		ABZEncoder.ANGLE_ROTOR_INT_OLD = 42010;
		EXTI_ClearITPendingBit(EXTI_Line15);
	}
}

int ArrayEqual(uint8_t A[],uint8_t B[],uint8_t AT[],uint8_t BT[],uint8_t size)
{
	for(int i =0; i<size; i++)
	{
		if(A[i]!=AT[i] || B[i] != BT[i])
		{
			return 0 ;
		}
	}
	return 1;
}

void PosCal(void)
{
		ABZEncoder.vari = ABZEncoder.direction * (ABZEncoder.AB_count_new - ABZEncoder.AB_count_old);
    if(abs(ABZEncoder.vari)>2048)
    {
        if(ABZEncoder.vari > 0 )
        {
            ABZEncoder.vari = - (4096 % ABZEncoder.vari);
        }
        else
        {
            ABZEncoder.vari =  (4096 % ABZEncoder.vari);
        }
    }
//		ABZEncoder.ANGLE_ROTOR = ABZEncoder.ANGLE_ROTOR_OLD + ABZEncoder.vari * 0.032213596545598f;
//		
//		if(ABZEncoder.ANGLE_ROTOR > 6.283185307179586f)
//		{
//			ABZEncoder.ANGLE_ROTOR -= 6.283185307179586f; 
//		}
//		else if (ABZEncoder.ANGLE_ROTOR < -6.283185307179586f)
//		{
//			ABZEncoder.ANGLE_ROTOR += 6.283185307179586f; 
//		}
//		ABZEncoder.ANGLE_ROTOR_OLD = ABZEncoder.ANGLE_ROTOR;
//
//		ABZEncoder.ANGLE_ROTOR_INT = ABZEncoder.ANGLE_ROTOR * 10430.3783;

//		 ABZEncoder.AB_count_old =  ABZEncoder.AB_count_new;
		
		ABZEncoder.ANGLE_ROTOR_INT = ABZEncoder.ANGLE_ROTOR_INT_OLD + ABZEncoder.vari * 336;
		ABZEncoder.AB_count_old = ABZEncoder.AB_count_new;
		ABZEncoder.ANGLE_ROTOR_INT_OLD = ABZEncoder.ANGLE_ROTOR_INT;
		
}


void PWMInitPosCal(void)
{
	ABZEncoder.InitPos_RAW = (testdutycycle * 4095);
	ABZEncoder.InitPos = (ABZEncoder.InitPos_RAW*21)%4096;  //浮点电角度 乘以系数 6.28/4096
	ABZEncoder.InitPos = ABZEncoder.InitPos * 16;				// 转定点电角度 乘以系数 65536/6.28，其实也就是65536/4096 = 16
//	ABZEncoder.ANGLE_ROTOR_OLD = (float) ABZEncoder.InitPos * 0.00009587379924285256f;
}


void SpeedCal(void)
{
	ABZEncoder.vari_S = ABZEncoder.direction *(ABZEncoder.AB_count_new - ABZEncoder.AB_S_Old);
    if(abs(ABZEncoder.vari_S)>2048)
    {
        if(ABZEncoder.vari_S > 0 )
        {
            ABZEncoder.vari_S = - (4096 % ABZEncoder.vari_S);
        }
        else
        {
            ABZEncoder.vari_S =  (4096 % ABZEncoder.vari_S);
        }
    }
		
		ABZEncoder.SPEED_ROTOR = ABZEncoder.vari_S * 0.244140625f;
		ABZEncoder.AB_S_Old = ABZEncoder.AB_count_new;
	
	
}

void speed_filter(void)
{
	if(fabs(ENCOD_PLL.SPEED_ROTOR) < 60.0f)
	{
		ENCOD_PLL.SPEED_ROTOR = 0;
	}
	ENCOD_PLL.SPEED_ROTOR /=60;
	static float speed_arr[10] = {0.0f};
	uint8_t i = 0, j = 0;
	float temp = 0.0;
	static uint8_t k = 0;
	
	ENCOD_PLL.SPEED_ROTOR_old = ENCOD_PLL.SPEED_ROTOR;
	speed_arr[k++] = ENCOD_PLL.SPEED_ROTOR_old;//保存电机转速
	if (k == 10)
	{
		for (i = 10; i >= 1; i--)//冒泡排序
		{
			for (j = 0; j < (i - 1); j++) 
			{
				if (speed_arr[j] > speed_arr[j + 1]) //数值比较 
				{ 
					temp = speed_arr[j];//数值换位
					speed_arr[j] = speed_arr[j + 1];
					speed_arr[j + 1] = temp;
				}
			}
		}
		temp = 0.0;
		for (i = 2; i < 8; i++)//去除两边高低数据
		{
			temp += speed_arr[i]; //将中间数值累加
		}
		temp /= 6;//求速度平均值
		k = 0;
		//一阶低通滤波
		//公式为：Y(n)= qX(n) + (1-q)Y(n-1)
		//其中X(n)为本次采样值；Y(n-1)为上次滤波输出值；Y(n)为本次滤波输出值，q为滤波系数
		//q值越小则上一次输出对本次输出影响越大，整体曲线越平稳，但是对于速度变化的响应也会越慢
		ENCOD_PLL.SPEED_ROTOR_FILT= (0.5f * temp) + (0.5f * ENCOD_PLL.SPEED_ROTOR_old);
	}	
}



void ABZInit(void)
{
	ABZEncoder.direction = 1;
	ABZEncoder.ANGLE_ROTOR_INT = 0;
	ABZEncoder.ANGLE_ROTOR_INT_OLD = 0;
	ABZEncoder.vari = 0;
	ABZEncoder.vari_S = 0;
	ABZEncoder.AB_count_new = 0;
	ABZEncoder.AB_count_old = 0;
	ABZEncoder.AB_S_Old = 0;
	ABZEncoder.ANGLE_ROTOR = 0;
	ABZEncoder.ANGLE_ROTOR_OLD = 0;
	ABZEncoder.InitPos = 0;
	ABZEncoder.SPEED_ROTOR = 0;
	ABZEncoder.SPEED_ROTOR_FILT = 0;
}

void AUTOCalib(void)
{
	uint16_t DirDtcOld = 0;
	static int16_t DirDiff = 0;
	
	if(ATOCALIB.DirDtc_FLAG == 0)
	{
		ATOCALIB.DirDtc_count++;
		if(ATOCALIB.DirDtc_count < 15000)
		{
				ILoopDrag_D(100,4.5);

			if(ATOCALIB.DirDtc_count == 10) //将10k分频为1k
			{
//				testdutycycle = IC_DUTYCYLE();
				PWMInitPosCal();

				DirDiff = ABZEncoder.InitPos_RAW - DirDtcOld;
				
					if(abs(DirDiff)>2048)
					{
							if(DirDiff > 0 )
							{
									DirDiff = - (4095 % DirDiff);
							}
							else
							{
									DirDiff =  (4095 % DirDiff);
							}
					}
				
				DirDtcOld = ABZEncoder.InitPos_RAW;
					ATOCALIB.DirDtc_count = 0;
			}
		}
		else
		{
			ATOCALIB.DirDtc_FLAG = 1;
			if(DirDiff > 0)
			{
				ATOCALIB.DirDetect = 1;
			}
			else if(DirDiff < 0)
			{
				ATOCALIB.DirDetect = -1;
			}
			else
			{
				PWMShut();
			}
		}
	}
	else
	{
		if(ATOCALIB.ATO_FLAG == 0&&ATOCALIB.ATO_Eturns == 0)
		{
			ZeroPosiSet(&M0,&M1);
			PWMStandby();
		}
		
		if(ATOCALIB.ATO_FLAG == 0&&ATOCALIB.ATO_Eturns < ATOCALIB.Poles&&ATOCALIB.ATO_Eturns > 0)
		{
			
			if(ATOCALIB.ATO_count < 2048)
			{
				ATOCALIB.ATO_count++;
				ILoopDrag_D(32,3);  //2048*32 = 65536,强拉的话差不多一个周期
			}
			else
			{
				ZeroPosiSet(&M0,&M1);
				if(ATOCALIB.ATO_Eturns == ATOCALIB.Poles)
				{
					ATOCALIB.ATO_FLAG = 1;
					PWMStandby();
					motor_status = Standby;
				}
			}
		}
	}
	
	
}

void ZeroPosiSet(MOTORController*M, MOTORController*N)
{
    static uint16_t prepos_flg = 0;

    prepos_flg++;

    if(prepos_flg <= 1000)
    {
        setPhaseVoltage(&M0,0,1.5,0);
        setPhaseVoltage(&M1,0,1.5,32768);
//			testdutycycle = IC_DUTYCYLE();
			PWMInitPosCal();
    }
    else if(prepos_flg > 1000)
    {
			
//			testdutycycle = IC_DUTYCYLE();
			PWMInitPosCal();
			ATOCALIB.PosiRec[ATOCALIB.ATO_Eturns] = ABZEncoder.InitPos_RAW;
			ATOCALIB.EroRec[ATOCALIB.ATO_Eturns] = ABZEncoder.InitPos_RAW%195;
			
			ATOCALIB.ATO_Eturns++;
			PWMStandby();
			ATOCALIB.ATO_count = 0;
			
			prepos_flg = 0;
    }
}
