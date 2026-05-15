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
#ifndef __CURRENTSENSE_H
#define __CURRENTSENSE_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "foc_utils.h"
#include "lowsideCurrentsense.h"
/* USER CODE BEGIN Private defines */
float getDCCurrent(MOTORController *M,float motor_electrical_angle);
DQCurrent_s getFOCCurrents(MOTORController *M,float angle_el);

extern DQCurrent_s currentM0,currentM1;
extern AlphBeta_s AlphBetaCurM0,AlphBetCuraIM1;
extern AlphBeta_s AlphBetaVolM0,AlphBetaVolM1;
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
