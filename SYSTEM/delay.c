
#include "delay.h"
#include "AS5600.h"
#include "pid.h"
#include "foc_utils.h"
#include "tamagawa.h"
#include "usart0.h"
#include "ABZ.h"
#include "OrthPLL.h"
#include  "Protection.h"
#include "Estimate.h"

#include "UserInc.h"
/******************************************************************************/
//配置为1ms中断
void systick_CountInit(void)
{
	  SysTick->LOAD = 168000;      //set reload register
    SysTick->VAL  = 0;
    SysTick->CTRL = SysTick_CTRL_CLKSOURCE_Msk | SysTick_CTRL_TICKINT_Msk | SysTick_CTRL_ENABLE_Msk; //时钟AHB不分频，中断使能，SysTick使能
		NVIC_SetPriority (SysTick_IRQn, 15);

}
/******************************************************************************/
uint32_t sysTickUptime;


float testLog = 0;
uint8_t pos_flag = 0;
uint8_t div5ms = 0;
uint16_t div1s = 0;

void BoardSense(void)
{
	div5ms++;
	if (div5ms == 5)  //5ms采集母线电压
	{

		
		BusVolSense();
		BusVolProt();
		div5ms = 0;
		
		div1s++;
		if(div1s == 200) //1s采集板子温度、还有定子温度
		{
			
//			StatorTemSense();
			BoardTemSense();
			div1s = 0;
//			HeatProt();
			
		}
	}
}
float PosError = 0;
uint8_t ZeroTamaComp = 0;

void TIM4_IRQHandler(void)
{
	if(TIM_GetFlagStatus(TIM4,TIM_IT_Update) == SET)
	{
//		GPIO_ToggleBits(TESTGPIO_Port,TESTGPIO_Pin);
		TIM_ClearFlag(TIM4, TIM_FLAG_Update);
		
//		if (TamaZerosFlag == 1)
//		{
//			ZeroTama();
//			ZeroTamaComp = 1;
//		}
//		else
//		{
//			tamagawa_read(DATA_ID_0);
//		}

//		if(position>360)
//		{
//			PWMShut();
//		}
	}

}

uint32_t timer_count = 0;
uint16_t smo_count = 0;
uint8_t smo_divide = 0;
void TIM5_IRQHandler(void)
{
	if(TIM_GetFlagStatus(TIM5,TIM_IT_Update) == SET)
	{
				TIM_ClearFlag(TIM5, TIM_FLAG_Update);
		Pgy_Slow_Loop();
//		GPIO_ToggleBits(TESTGPIO_Port,TESTGPIO_Pin);
//		GPIO_SetBits(LED_GPIO_Port,LED_Pin);
//		M0.P_ang.I = 5.23165f/4.21354f;
//		GPIO_ResetBits(LED_GPIO_Port,LED_Pin);
//		if (TamaZerosFlag == 1)
//		{
//			ZeroTama();
//		}
//		else
//		{
//			tamagawa_read(DATA_ID_0);
//		}


//		ENCOD_PLL.SPEED_ROTOR_FILT = ENCOD_PLL.SPEED_ROTOR_S;
		/*****无感测试******/
// 		if(motor_status == Running && sensorlessFlag == 0)
// 		{
//			SpeedRef = 7;
// 			if(AngleAdd<200)
// 			{
//				if(smo_divide == 4)
//				{
//					AngleAdd ++;
//					smo_divide = 0;
//				}
//				else
//				{
//					smo_divide++;
//				}
// 			}
// 			else
// 			{
// 				AngleAdd = 200;
// 				smo_count++;
// 				if(smo_count>1000)
// 				{
// 					sensorlessFlag = 1;
//// 					SpeedRef = 8;
// 				}
// 			}
// 		}


// 	if(sensorlessFlag)
// 	{
// 		Climbing(&M0,SpeedRef);
// //		ENCOD_PLL.SPEED_ROTOR_FILT = SMO_Esti.SmoPLL.PLL_SpeedOut_now*0.016667;
// 		PLLSpeedCal();
// 	}
// 	else
// 	{
// //	SpeedLimitClimb(&M0,&M1);
//		
//		

// 	/**计算速度与滤波,电机旋转方向**/

//		
// 		ENCOD_PLL.SPEED_ROTOR_FILT = SMO_Esti.SmoPLL.PLL_SpeedOut_now*0.016667f;
// 		M0.PID_vel.ref = 7;
// 		PLLSpeedCal();
// 	}

           /*****无感测试******/

	/**计算速度与滤波,电机旋转方向**/
		Climbing(&M0,SpeedRef);
//		EAngleSpeedCal();
//		PLLSpeedCal();
	
	
	/**主模式选择**/
	if(motor_status == Running || motor_status == AxisHallCalib)
	{
//		timer_count++;
//		if(timer_count % 20000 == 0)
//		{
//			FPV_KP = 0.6;
//			M0.P_ang.ref = 10;
//		}
//		else if(timer_count % 20000 == 10000)
//		{
//			FPV_KP = 0.6;
//			M0.P_ang.ref = -10;
//		}
		
//		Climbing_POS(&M0,20);

		switch(M0.controller)
		{
			/**力位混合模式**/
			case Type_FPVc: 
			{
				static float TorqueOut = 0;
				/***力位混合测试限定参数数据***/
//				T_I_Ratio = 0.2;
				M0.PID_vel.limit = 20;
				M0.P_ang.limit = 20;
//				TorqueRef = 0;
				/***力位混合测试限定参数数据***/
				M0.P_ang.I = 0;
				M0.P_ang.D = 0;
				M0.PID_vel.I = 0;
				M0.PID_vel.D = 0;
				
				M0.PID_vel.P = (float)FPV_KD*0.0000610351562;
				M0.P_ang.P = (float)FPV_KP*0.0000610351562;
//				M0.PID_vel.P = FPV_KD;
//				M0.P_ang.P = FPV_KP;
				
				M0.PID_vel.ref = M0.PID_vel.ref*0.001627604166f;
				M0.P_ang.ref = M0.P_ang.ref*0.0039062f;
				
				//最优旋转方向选择
				PosError = (float)M0.P_ang.ref - position;
//				PosError = (float)M0.P_ang.ref - position;
//				if(fabs(PosError)>180)
//				{
//					if(PosError > 0)
//					{
//						PosError = PosError - 360;
//					}
//					else
//					{
//						PosError = 360 + PosError;
//					}
//				}
				
				
				/**速度环1kHz频率**/
					M0.PID_vel.output = PIDoperator(&M0.PID_vel,((float)M0.PID_vel.ref - ENCOD_PLL.SPEED_ROTOR_FILT),Ts_V);

				/**位置环分频500Hz频率**/
//					pos_flag++;
//				if(pos_flag == 2)
//				{
					
					M0.P_ang.output =  PIDoperator(&M0.P_ang,(PosError),Ts_V);
//					pos_flag = 0;
//				}
				TorqueOut = (M0.P_ang.output + M0.PID_vel.output + TorqueRef);
				TorqueOut = _constrain(TorqueOut, -125, 125);     //125*0.2 = 25A
				ActualTorque = TorqueOut*1024;//  /T_I_Ratio*1024;
				/**q轴电流指令 = （位置环输出 + 转速换输出 + 指令转矩）/（电流力矩常数）**/
//				M0.PID_q.ref =  ActualTorque* T_I_Ratio;
				M0.PID_q.ref = TorqueOut * T_I_Ratio;
				M1.PID_q.ref = M0.PID_q.ref;//两套绕组的速度环和位置环一样，指令转矩均分给两套绕组
				
				break;
			}
			
			/**力矩模式**/
			case Type_torque: 
			{
				/**力矩模式仅改变q轴的参考电流，实际环路频率为主中断频率**/
				
				/**q轴电流指令 = 测出来的TI比例是用两套绕组通入同样的电流**/
				M0.PID_q.ref = TorqueRef * T_I_Ratio ;
				M1.PID_q.ref = TorqueRef * T_I_Ratio ;
				
				break;
			}
			
			
			
			/**速度模式**/
			case Type_velocity:  //速度模式
			{
				/**速度环1kHz频率**/

					M0.PID_vel.output = PIDoperator(&M0.PID_vel,(M0.PID_vel.ref - ENCOD_PLL.SPEED_ROTOR_FILT),Ts_V);
				
				/**q轴电流指令 = 速度环输出**/
				M0.PID_q.ref = M0.PID_vel.output;
				
				break;
			}
			
			
			/**位置模式**/
			case Type_angle: //位置模式
			{

				/**位置环分频500Hz频率**/
					pos_flag++;
				if(pos_flag == 2)
				{
					M0.P_ang.output = PIDoperator(&M0.P_ang,(M0.P_ang.ref - ENCODER.ANGLE_ROTOR_Rdu),2e-3);
				}

				/**速度环1kHz频率**/
				
					M0.PID_vel.ref = M0.P_ang.output;//位置环输出作为速度环参考
					speedcapt();
					speed_filter(); //速度滤波
					M0.PID_vel.output = PIDoperator(&M0.PID_vel,(M0.PID_vel.ref - ENCODER.SPEED_ROTOR_FILT),1e-3);
				
				/**q轴电流指令 = 速度环输出**/
				M0.PID_q.ref = M0.PID_vel.output;
				M1.PID_q.ref = M0.PID_vel.output;
				
				break;
			}
		}
		
	}

	}
}

//嘀嗒中断频率1k没问题，但是有积累误差偏移
void SysTick_Handler(void)  //systick中断入口函数
{
//	delay_us(110);
//	GPIO_ToggleBits(TESTGPIO_Port,TESTGPIO_Pin);
	sysTickUptime++;
	/**检测母线电压、温度**/

	BoardSense();
	
	/**速度爬坡指令转化、速度分段限幅**/


}
/******************************************************************************/
//最多计时0xFFFFFFFF = 4294967295 us = 4294.9秒 = 71.58分，计满以后循环从0开始
uint32_t _micros(void)
{
	uint32_t  ms,cycle_cnt;
	
	do{
		ms = sysTickUptime;
		cycle_cnt = SysTick->VAL;
	} while (ms != sysTickUptime);
	
	return (ms * 1000) + (168000000 - cycle_cnt) / 168000;
}
/******************************************************************************/
//延时nus
void delay_us(unsigned long nus)
{
	uint32_t  t0,t1,diff;
	
	t0 = _micros();
	do{
		t1=_micros();
		if(t1>=t0)diff=t1-t0;
		else diff=0xffffffff-t0+t1;
	}while(diff < nus);
}
/******************************************************************************/
//延时nms
void delay_ms(unsigned long nms)
{
	uint32_t  t0,t1,diff;
	
	t0 = _micros();
	do{
		t1=_micros();
		if(t1>=t0)diff=t1-t0;
		else diff=0xffffffff-t0+t1;
	}while(diff < nms * 1000);
}
/******************************************************************************/
