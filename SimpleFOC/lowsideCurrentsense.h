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
#ifndef __LOWSIDECURRENTSENSE_H
#define __LOWSIDECURRENTSENSE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "foc_utils.h"

/* USER CODE BEGIN Private defines */
void LowsideCurrentSense(MOTORController *M,float _shunt_resistor, float _gain, int _pinA, int _pinB, int _pinC);
void ADC_Init_synch(void);
//void LowsideCurrentSense_Init(MOTORController *M);
void CurrentSenseRaw(MOTORController *M,MOTORController *N);
void PhaseCurrent(MOTORController *M,PhaseCurrent_s*current);
void Current_calibrateOffsets(MOTORController *M, MOTORController*N);
void PWMShut(void);

PhaseCurrent_s getPhaseCurrents(MOTORController *M);

extern PhaseCurrent_s PhasecurrentM0,PhasecurrentM1;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
