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
#ifndef __BLDCMOTOR_H
#define __BLDCMOTOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "foc_utils.h"
#include "pid.h"
#include "CurrentSense.h"
#include "Sensor.h"
#include "FOCMotor.h"

/* USER CODE BEGIN Private defines */
extern  float voltage_power_supply;
/******************************************************************************/
void loopFOC(MOTORController *M);
void setPhaseVoltage(MOTORController *M,float Uq, float Ud, float angle_el);
void motorPrepos(MOTORController*M, MOTORController*N);
void VolMotorPrepos(MOTORController*M, MOTORController*N);

extern uint16_t Pa_PWM ;
extern uint16_t Pb_PWM ;
extern uint16_t Pc_PWM ;
extern uint16_t Pd_PWM ;
extern uint16_t Pe_PWM ;
extern uint16_t Pf_PWM ;
extern DQCurrent_s current;

extern float test;

extern float PowerOutput;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
