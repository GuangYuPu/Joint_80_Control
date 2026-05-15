/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __FOC_UTILS_H
#define __FOC_UTILS_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "stm32f4xx.h"
#include "usart.h"
#include "delay.h"
//#include "tim.h"
//#include "i2c.h"
//#include "spi3.h"
#include "adc.h"
#include "dma.h"
//#include "foc_utils.h"
//#include "MagneticSensor.h"
//#include "Encoder.h"
//#include "Sensor.h"
//#include "BLDCMotor.h"
//#include "FOCMotor.h"
//#include "lowpass_filter.h"
//#include "pid.h"
//#include "LowsideCurrentSense.h"
//#include "CurrentSense.h"






typedef struct
{
    float hCos;
    float hSin;
}
Trig_Components;

extern Trig_Components MCM_Trig_Functions(int16_t hAngle);
extern const int16_t hTEMPERATURE_TABLE[160];
extern const int16_t hStatorTEMP_TABLE[241] ;
/* USER CODE BEGIN Private defines */


/******************************************************************************/
// sign function
#define _sign(a) ( ( (a) < 0 )  ?  -1   : ( (a) > 0 ) )
#define _round(x) ((x)>=0?(long)((x)+0.5f):(long)((x)-0.5f))
#define _constrain(amt,low,high) ((amt)<(low)?(low):((amt)>(high)?(high):(amt)))
#define _sqrt(a) (_sqrtApprox(a))
#define _isset(a) ( (a) != (NOT_SET) )

// utility defines
#define _2_SQRT3 1.15470053838f
#define _SQRT3 1.73205080757f
#define _1_SQRT3 0.57735026919f
#define _SQRT3_2 0.86602540378f
#define _SQRT2 1.41421356237f
#define _120_D2R 2.09439510239f
#define _PI 3.14159265359f
#define _PI_2 1.57079632679f
#define _PI_3 1.0471975512f
#define _2PI 6.28318530718f
#define _3PI_2 4.71238898038f
#define _PI_6 0.52359877559f

#define NOT_SET -12345


#define Standby 0  //待机状态
#define ADCOfst 1  //上机预备状态
#define PrePos  2  //预定位状态（没用到）
#define Running 3  //运行状态
#define ERROR   4  //错误状态
#define AtoCalib 5 //自动机械位置偏差强拉矫正
#define ZeroCurCalib 6 //手动机械位置零电流反电势矫正
// #define AxisHallCalib 7 //输出端位置矫正
// #define WriteToFlash 8 //将数据保存进入Flash

#define PeakCurOver 1;
#define MeanCurOver 2;
#define PosStuck 3;
#define BoardOvrHeat 4;
#define StatorOvrHeat 5;
#define BusVolOver  6;
#define BusVolUnder 7;

#define IFCurrent 6.0f;  //预定位强拉电流
#define CurrentLoopAngleAddInit 5.0f; //电流自增角度强拉电流
/******************************************************************************/

// phase current structure
typedef struct
{
    float a;
    float b;
    float c;
} PhaseCurrent_s;

typedef struct 
{
	float alph;
	float beta;
} AlphBeta_s;

typedef struct
{
    float d;
    float q;
} DQCurrent_s;
// dq voltage structs
typedef struct
{
    float d;
    float q;
} DQVoltage_s;
/******************************************************************************/
typedef struct
{
    float Tf; //!< Low pass filter time constant
    float y_prev; //!< filtered value in previous execution step
    unsigned long timestamp_prev;  //!< Last execution timestamp
} LowPassFilter;

typedef struct
{
    float P; //!< Proportional gain
    float I; //!< Integral gain
    float D; //!< Derivative gain
    float output_ramp; //!< Maximum speed of change of the output value
    float limit; //!< Maximum output value
    float error_prev; //!< last tracking error value
    float output_prev;  //!< last pid output value
    float integral_prev; //!< last integral component value
    unsigned long timestamp_prev; //!< Last execution timestamp
    float ref;
    float output;
} PIDController;


typedef enum
{
    Type_torque,//!< 力矩模式
    Type_velocity,//速度模式
    Type_angle,//位置模式
    Type_FPVc,//力位混合模式
} MotionControlType;



/******************************************************************************/
typedef struct
{
    int  enable;     //电机使能状态 1使能，0不使能
    char id;         //=0为M0,=1为M1
    char str[4];     //定义结构体的名字 "M1"或者"M2"


    float voltage_limit;
    int  pole_pairs; //极对数，轮毂电机为10对极
    float velocity_limit; //
    float current_limit;
    unsigned long open_loop_timestamp;


    LowPassFilter  LPF_q,LPF_d,LPF_vel;
    PIDController  PID_q,PID_d,PID_vel,P_ang;

    int pinA,pinB,pinC,pinD,pinE,pinF;
    float gain_a,gain_b,gain_c,gain_d,gain_e,gain_f; //电流采样放大倍数
    uint32_t offset_suma,offset_sumb,offset_sumc;    //电流采样上电检查时累加和
    uint16_t offset_ia,offset_ib,offset_ic;         //电流采样偏置
    uint16_t phA,phB,phC;                            //相电流原始数据
		uint16_t Encoder_x,Encoder_y,Encoder_z;
    float IProt;                                     //相电流保护

    MotionControlType controller;


    int16_t electrical_angle_int;


} MOTORController;

extern MOTORController M0,M1;
extern int8_t motor_status;
extern int32_t TorqueRef;
extern float T_I_Ratio;
extern float voltage_power_supply;
extern float SpeedRef ;
extern int16_t BoardTemp;
extern int16_t StatorTemp;
extern int32_t FPV_KP;
extern int32_t FPV_KD;
extern uint8_t TamaZerosFlag;
extern int32_t ActualTorque;
extern float PosUpload ;
uint32_t sqrt32(uint32_t x);
/******************************************************************************/
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
