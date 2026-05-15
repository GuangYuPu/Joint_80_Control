#ifndef __ANGLEE_ESTI_H
#define __ANGLEE_ESTI_H


#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "foc_utils.h"
#include "CurrentSense.h"
#include "BLDCMotor.h"
#include "OrthPLL.h"
#define RangeLimit(min,x,max)  (((x) >= (max))?(max):(((x)>= (min))?(x):(min)))

typedef struct//锁相环PI
{
	
	float Ref;
	float Actual;	
	float Err_now;
	float Kp;
	float Ki;
	float Pout;
	float Iout;
	float I_limit;
	float Out_limit;	
	float Output;
  int16_t PLL_SpeedOut_now;
	int16_t PLL_SpeedOut_last;
	int16_t PLL_Speed;
	int16_t PLL_AngleOut;
	int16_t PLL_AngleOut_MINT;
	uint16_t PLL_AngleOut_MINTCalib;
	
}PLL;

void PLL_Calc(PLL *v);//锁相环PI运算


typedef struct//
{
	PLL SmoPLL;

	float CurrGain;	
	float VoltGain; 	
	float SgnGain;
	
	float preUalpha;
	float preUbeta;
	float preIalpha;
	float preIbeta;
	
	float EstiIalpha;
	float EstiIbeta;
	
	float BEMF_Alpha;
	float BEMF_Beta;
	
	float BEMF_1st_Alpha;
	float BEMF_1st_Beta;//滑模估算后进行一次低通滤波后的值
	
	float BEMF_2nd_Alpha;
	float BEMF_2nd_Beta;//二次滤波，看情况使用
	
	float BEMF_SinValue;
	float BEMF_CosValue;
	float Angle_SinValue;
	float Angle_CosValue;
	
	int32_t Diffree;
		
}SMO_Observer;

void Pos_Esti_Init(void);//锁相环初始化+观测器初始化
void Pos_Esti_Calc(void);//观测器运算

void Anglee_Diff(void);//角度差估计

extern SMO_Observer SMO_Esti;







#ifdef __cplusplus
}
#endif

#endif 

