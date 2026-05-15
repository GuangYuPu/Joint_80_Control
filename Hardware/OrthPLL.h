#ifndef __ORTHPLL_H
#define __ORTHPLL_H


#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx.h"
#include "foc_utils.h"
#include "BLDCMotor.h"




typedef struct
{
    int16_t turns; //电机转动圈数，正转正数，反转负数
    int8_t direction;  //电机转动方向 1正转，-1反转
	
	
		uint32_t InitPos_RAW;
		int16_t InitPos; //PWM模式判断;
		uint16_t PosCalib;//机械矫正角度；
		uint16_t PosCalibOld;
		
	
		int16_t  ADC_X;
		int16_t  ADC_Y;

		int16_t  ADC_W;
		int16_t  ADC_Z;
	
		PIDController PLL;
	
		float sineValue;
		float cosValue;
		
					//电机转速判断计数
		
		
    float ANGLE_ROTOR;			//电机浮点位置(电角度)
		float ANGLE_ROTOR_OLD;
    float ANGLE_ROTOR_Rdu; //轮盘浮点位置

    int16_t ANGLE_Rducer_RAW;

    int16_t ANGLE_ROTOR_INT; //电机定点位置（查表算三角函数用）
		int16_t ANGLE_ROTOR_MINT;
		int16_t ANGLE_ROTOR_MINT1;
		uint16_t ANGLE_ROTOR_MINT_Calib;
		uint16_t ANGLE_ROTOR_MINT_Calib_E;
		uint16_t ANGLE_ROTOR_MINT_Calib_OLD;
		int16_t ANGLE_ROTOR_INT_OLD;
		uint16_t ANGLE_ROTOR_EINT_Calib;
		uint16_t EangleLast;
		int32_t vari_Eangle;
		int32_t Angle_E_sum;
		int32_t Angle_E_sum_last;

		float INC_COEF;
		int16_t vari_s;
    float SPEED_ROTOR;		
		float SPEED_ROTOR_S;
    float SPEED_ROTOR_Rdu;	//轮盘转速

    float SPEED_ROTOR_old;

    float SPEED_ROTOR_FILT; //电机转速
} ENCOD_PLL_T;


typedef struct
{
    int16_t turns; //电机转动圈数，正转正数，反转负数
    int8_t direction;  //电机转动方向 1正转，-1反转
	
	
		uint32_t InitPos_RAW;
		int16_t InitPos; //PWM模式判断;
		uint16_t PosCalib;//机械矫正角度；
		uint16_t PosCalibOld;
		
	
		float  ADC_X;
		float ADC_X_1st;
		float ADC_X_2st;
		float ADC_Y;
		float ADC_Y_1st;
		float ADC_Y_2st;
	
		PIDController PLL_tst;
	
		float sineValue;
		float cosValue;
		
					//电机转速判断计数
		
		
    float ANGLE_ROTOR;			//电机浮点位置(电角度)
		float ANGLE_ROTOR_OLD;
    float ANGLE_ROTOR_Rdu; //轮盘浮点位置

    int16_t ANGLE_Rducer_RAW;

    int16_t ANGLE_ROTOR_INT; //电机定点位置（查表算三角函数用）
		int16_t ANGLE_ROTOR_MINT;
		uint16_t ANGLE_ROTOR_MINT_Calib;
		uint16_t ANGLE_ROTOR_MINT_Calib_OLD;
		int16_t ANGLE_ROTOR_INT_OLD;


		float INC_COEF;
		int16_t vari_s;
    float SPEED_ROTOR;			
    float SPEED_ROTOR_Rdu;	//轮盘转速

    float SPEED_ROTOR_old;

    float SPEED_ROTOR_FILT; //电机转速
} ENCOD_PLL_TST;


typedef struct
{
		uint8_t Poles;
		uint16_t DirDtc_count;
		uint16_t DirPosCount;
		uint16_t DirNegCount;
		uint8_t DirDtc_FLAG; //方向验证标志位
		int8_t DirDetect; //编码器方向与电正方向，1为一致，-1为相反；
		
		uint8_t ATO_FLAG;  //矫正完成标志位；

		uint16_t ATO_count; //旋转计算；
		uint8_t ATO_Eturns; //电角度圈数记录；
		
	  uint16_t PosiRec[21]; //位置记录；
		int16_t  EroRec[21]; //差别记录；
	
		int16_t EroMeanRec; //平均误差；
	
	uint16_t Y_Ofst;
	uint16_t Y_Amp;
	float Y_Amp_f ;  //10000


	uint16_t X_Ofst;
	uint16_t X_Amp;
	float X_Amp_f;
		
} OrthATOCALIB_T;


extern ENCOD_PLL_T ENCOD_PLL;
extern OrthATOCALIB_T OrthATOCALIB;
extern int16_t DirDiff ;
extern ENCOD_PLL_TST EMFAngleTst;
extern int32_t calibREC;

void Orth_CAL(ENCOD_PLL_T * V);
void PLL_Cal(PIDController* PID,float error,float Tstep);
void OrthPLLInit(void);
void PLLSpeedCal(void);
void OrthZeroPosiSet(MOTORController*M, MOTORController*N);
void OrthAUTOCalib(void);
void OrthPLLInit_tst(void);
void Orth_CAL_f(ENCOD_PLL_TST * V);
void PLL_Cal_TST(PIDController* PID,float error,float Tstep);
void PLLSpeedCalEangle(void);
void EAngleSum(void);
void EAngleSpeedCal(void);
#ifdef __cplusplus
}
#endif
#endif
