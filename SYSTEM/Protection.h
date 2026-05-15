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
#ifndef __PROTECTION_H
#define __PROTECTION_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "foc_utils.h"

/* Private includes ----------------------------------------------------------*/
void PWMShut(void);
void ACCurrentProt(MOTORController* M,PhaseCurrent_s*I0);
void PWMStandby(void);
void PWMEnable(void);
void PWMDisable(void);

void HeatProt(void);
void BusVolProt(void);
void MosShut(void);
void TimGpioReInit(void);

extern uint8_t PWMENflag;
extern uint8_t TimGpioReFlag;
extern uint8_t MosShutFlag;

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
