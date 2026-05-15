
#ifndef _ABZ_H
#define _ABZ_H
#include "stm32f4xx.h"
#include "foc_utils.h"
#include "adc.h"
/******************************************************************************/ 



extern uint32_t CptrA ;
extern uint32_t CptrB ;
extern uint32_t AB_Count;
extern int16_t Diff ;





typedef struct
{
    int16_t turns; //电机转动圈数，正转正数，反转负数
    int8_t direction;  //电机转动方向 1正转，-1反转
	
	
		uint32_t InitPos_RAW;
		int16_t InitPos; //PWM模式判断;
		uint16_t PosCalib;//机械矫正角度；
		
    int16_t vari;
    uint16_t AB_count_old;  //电机位置判断计数
    uint16_t AB_count_new;
		
	
					//电机转速判断计数
		uint16_t AB_S_Old;
		int16_t vari_S;
	
	
    float ANGLE_ROTOR;			//电机浮点位置
		float ANGLE_ROTOR_OLD;
    float ANGLE_ROTOR_Rdu; //轮盘浮点位置

    int16_t ANGLE_Rducer_RAW;

    int16_t ANGLE_ROTOR_INT; //电机定点位置（查表算三角函数用）
		int16_t ANGLE_ROTOR_INT_OLD;


    float SPEED_ROTOR;			
    float SPEED_ROTOR_Rdu;	//轮盘转速

    float SPEED_ROTOR_old;

    float SPEED_ROTOR_FILT; //电机转速
} ABZEncoder_T;

typedef struct
{
		uint8_t Poles;
		uint16_t DirDtc_count;
		uint8_t DirDtc_FLAG; //方向验证标志位
		uint8_t DirDetect; //编码器方向与电正方向，1为一致，-1为相反；
		
		uint8_t ATO_FLAG;  //矫正完成标志位；

		uint16_t ATO_count; //旋转计算；
		uint8_t ATO_Eturns; //电角度圈数记录；
		
	  uint16_t PosiRec[20]; //位置记录；
		int16_t  EroRec[20]; //差别记录；
		
} ATOCALIB_T;

extern ABZEncoder_T ABZEncoder;
extern ATOCALIB_T ATOCALIB;
/******************************************************************************/
void PosCal(void);
void EXTIB_Configuration(void);
int ArrayEqual(uint8_t A[],uint8_t B[],uint8_t AT[],uint8_t BT[],uint8_t size);
void SpeedCal(void);
void PWMInitPosCal(void);
void speed_filter(void);
void ABZInit(void);
void ZeroPosiSet(MOTORController*M, MOTORController*N);
/******************************************************************************/
#endif
