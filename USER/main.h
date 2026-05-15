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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define SYS_FREQ 168000000
#define PWM_FREQ 20000
#define FOCDiv 1  //ÖØ¸´¼ÆÊýÆ÷
//#define Ts1 1/PWM_FREQ
//#define Ts2 1e-3
//#define Ts3 5e-3
#define LED_Pin GPIO_Pin_12
#define LED_GPIO_Port GPIOB
#define RS485_EN_Pin GPIO_Pin_10
#define RS485_EN_GPIO_Port GPIOD
#define TESTGPIO_Port GPIOC
#define TESTGPIO_Pin GPIO_Pin_15
/* USER CODE BEGIN Private defines */
#define PWM_Period SYS_FREQ/PWM_FREQ/2
/* USER CODE END Private defines */
extern float Ts_I ;
extern float Ts_V ;
extern float Ts_P ;
#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
