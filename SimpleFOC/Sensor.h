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
#ifndef __SENSOR_H
#define __SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "foc_utils.h"
/* USER CODE BEGIN Private defines */
#define  AS5600_CPR       4096       //12bit
#define  AS5047P_CPR      16384      //14bit
#define  TLE5012B_CPR     32768      //15bit
#define  MA730_CPR        65536      //14bit,左对齐,低两位补0,所以是65536
#define  MT6701_CPR       65536      //14bit,左对齐,低两位补0,所以是65536
/******************************************************************************/
void MagneticSensor_Init(void);
float getAngle(MOTORController *M);
float getVelocity(MOTORController *M);
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
