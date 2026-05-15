
#include "MyProject.h"
#include "OrthPLL.h"
#include "Protection.h"
//void Current_calibrateOffsets(MOTORController *M);

PhaseCurrent_s PhasecurrentM0,PhasecurrentM1;

/******************************************************************************/

void LowsideCurrentSense(MOTORController *M,float _shunt_resistor, float _gain, int _pinA, int _pinB, int _pinC)
{
	float volts_to_amps_ratio;
	
	M->pinA = _pinA;
	M->pinB = _pinB;
	M->pinC = _pinC;
	
//	volts_to_amps_ratio = 0.00080566f /_shunt_resistor / _gain; // 
	volts_to_amps_ratio =  0.04029304f;
	M->gain_a = volts_to_amps_ratio;
	M->gain_b = volts_to_amps_ratio;
	M->gain_c = volts_to_amps_ratio;
	
	//printf("gain_a:%.2f,gain_b:%.2f,gain_c:%.2f.\r\n",gain_a,gain_b,gain_c);  //必须屏蔽，这句话打印会导致下面的函数执行不正常，一下午没有找到原因
}
/******************************************************************************/

void ADC_Init_synch(void)
{
            //配置DMA模式，转换温度检测和vbus，温度检测在simpleFOC中没有使用，开始转换

	//HAL_TIM_Base_Start(&htim2);
		
    TIM_Cmd(TIM1, ENABLE);
//		TIM_Cmd(TIM8, ENABLE);
		PWMENflag = 1;
	
		TIM_Cmd(TIM5, ENABLE);
		TIM_Cmd(TIM4, ENABLE);
		
//		MosShut();
	
	
	
	
	motor_status = ADCOfst;
	

	

}
/******************************************************************************/
//void LowsideCurrentSense_Init(MOTORController *M)
//{

//	Current_calibrateOffsets(M); //检测偏置电压，也就是电流0A时的运放输出电压值，理论值=1.65V
//}
/******************************************************************************/
// Function finding zero offsets of the ADC
void Current_calibrateOffsets(MOTORController *M, MOTORController*N)
{
	//母线电压一上电就有，不进行矫正
	static uint16_t ad_flag = 0;
	
//	M->offset_ia=0;
//	M->offset_ib=0;
//	M->offset_ic=0;
	
	// read the adc voltage 1000 times ( arbitrary number )
	if(ad_flag<256)
	{

		 M->offset_suma += M->phA;
		 M->offset_sumb += M->phB;
		 M->offset_sumc += M->phC;
		
//		I2C_DMA_Read(SLAVE_ADDR, ANGLE_REGISTER_HIGH_ADDR,ENCODER.buf, (u8*)(sizeof(ENCODER.buf)));
//		HAL_I2C_Mem_Read_DMA(&hi2c2,SLAVE_ADDR, ANGLE_REGISTER_HIGH_ADDR, I2C_MEMADD_SIZE_8BIT, ENCODER.buf_s, sizeof(ENCODER.buf_s));

	}
	else
	{
	// calculate the mean offsets
		 M->offset_ia = M->offset_suma>>8;
		 M->offset_ib = M->offset_sumb>>8;
		 M->offset_ic = M->offset_sumc>>8;
		ENCOD_PLL.ANGLE_ROTOR_MINT_Calib_OLD = ENCOD_PLL.ANGLE_ROTOR_MINT_Calib;
//				ENCODER_Get();
//				ENCODER.angle_old = ENCODER.ANGLE_Calib;
				motor_status = Standby;
		
				TIM1->CCR1=0;
        TIM1->CCR2=0;
        TIM1->CCR3=0;

//	
//				TIM8->CCR1=0;
//        TIM8->CCR2=0;
//        TIM8->CCR3=0;
		
//				TIM1->CCR1=0.2*PWM_Period;
//        TIM1->CCR2=0.4*PWM_Period;
//        TIM1->CCR3=0.6*PWM_Period;
//	
//				TIM8->CCR1=0;
//        TIM8->CCR2=0;
//        TIM8->CCR3=0;
	}

		ad_flag++;
}
/******************************************************************************/
// read all three phase currents (if possible 2 or 3)
PhaseCurrent_s getPhaseCurrents(MOTORController *M)
{
	PhaseCurrent_s current;
	
	current.a = (!_isset(M->pinA)) ? 0 : (M->phA - M->offset_ia)*M->gain_a; // amps
	current.b = (!_isset(M->pinB)) ? 0 : (M->phB - M->offset_ib)*M->gain_b; // amps
	current.c = (!_isset(M->pinC)) ? 0 : (M->phC - M->offset_ic)*M->gain_c; // amps
	
	return current;
}

void PhaseCurrent(MOTORController *M,PhaseCurrent_s*I)
{
	
	I->a = (M->phA - M->offset_ia)*M->gain_a; // amps
	I->b = (M->phB - M->offset_ib)*M->gain_b; // amps
	I->c = (M->phC - M->offset_ic)*M->gain_c; // amps
	
}

void CurrentSenseRaw(MOTORController *M,MOTORController *N)
{

}

