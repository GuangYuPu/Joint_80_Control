/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "string.h"
#include "delay.h"
#include <string.h>
#include "MyProject.h"
// #include "user.h"
#include "stdio.h"

#include "lowsideCurrentsense.h"
#include "BLDCMotor.h"
#include "Protection.h"
#include "tamagawa.h"
#include "AS5600.h"
#include "MotorInit.h"
#include "ABZ.h"
#include "FlashRW.h"
#include "OrthPLL.h"
#include "Estimate.h"
#include "UserInc.h"
uint8_t data_id0;

/// EtherCAT
void commander_run(void);

float Votagle_bus;
void GPIO_Configure(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOH, ENABLE);

    GPIO_InitStructure.GPIO_Pin   = LED_Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    // 推挽复用输出
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL; // 上拉
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(LED_GPIO_Port, &GPIO_InitStructure);
    GPIO_SetBits(LED_GPIO_Port, LED_Pin);

    GPIO_InitStructure.GPIO_Pin   = RS485_EN_Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    // 推挽复用输出
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL; // 上拉
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(RS485_EN_GPIO_Port, &GPIO_InitStructure); // 亮
    GPIO_ResetBits(RS485_EN_GPIO_Port, RS485_EN_Pin);

    GPIO_InitStructure.GPIO_Pin   = TESTGPIO_Pin;
    GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;    // 推挽复用输出
    GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL; // 上拉
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(TESTGPIO_Port, &GPIO_InitStructure); // 亮
    GPIO_SetBits(TESTGPIO_Port, TESTGPIO_Pin);
}
/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
float Ts_I = 5e-5;
float Ts_V = 1e-3;
float Ts_P = 5e-3;
float positionrec;
int main(void)
{
    GPIO_Configure();
    systick_CountInit(); // systick时钟开启1ms中断模式

    ADC_Common_Init(); // 初始化ADC的引脚和通用配置，设置中断
    ADC3_DMA_Init();   // 这个初始化放下面函数的后面会出现错位问题
    ADC2_TRGO_Init();  // 配置
    ADC1_TRGO_Init();  // 配置

    //    USART2_Init(115200);
    TIM4_Init();
    USART0_Init(1152000);
    TIM1_PWM_Init(); // M0接口，完成配置，但没有使能
    TIM5_Init();

    /*** FLASH读取初始位置***/
    read_from_flash(&OrthATOCALIB);
    //	OrthATOCALIB.EroMeanRec = -17445;
    MotorInit();      // 电机初始化
    ADC_Init_synch(); // ADC初始化和定时器同步
    Pos_Esti_Init();
    OrthATOCALIB.DirDetect = 1;
    
    //---------------User_Init_Begin----------
    Pgy_Init();
    //---------------User_Init_Finish----------------
    
    /***主动写入FLASH***/

    while (1) {
        //			printf_DMA("ANGLE:%d,%d,%d\n",Pa_PWM,Pb_PWM,Pc_PWM);
        //			printf_DMA("ANGLE:%d\n",DirDiff);
        //        			printf_DMA("ANGLE:%d,%d\n",M0.phA,M0.phC);
        //        			printf_DMA("ANGLE:%f,%f,%f\n",PhasecurrentM0.a,PhasecurrentM0.b,PhasecurrentM0.c);
        //			printf_DMA("ANGLE:%f,%f\n",PhasecurrentM0.a,currentM0.d);
        //			printf_DMA("ANGLE:%d\n",ENCOD_PLL.ANGLE_ROTOR_INT);
        //        printf_DMA("ANGLE:%d,%d,%f,%f\n", ENCOD_PLL.ANGLE_ROTOR_INT,SMO_Esti.SmoPLL.PLL_AngleOut,ENCOD_PLL.SPEED_ROTOR_FILT,ENCOD_PLL.SPEED_ROTOR_S);
        //			        printf_DMA("ANGLE:%f,%f,%f\n", ENCOD_PLL.SPEED_ROTOR_S,currentM0.q,M0.PID_vel.output);

        // printf_DMA("ANGLE:%f,%f,%f,%d\n", ENCOD_PLL.SPEED_ROTOR_FILT, currentM0.q, M0.PID_vel.output, ENCOD_PLL.ADC_Z);
        // printf_DMA("ANGLE:%f,%d,%d\n", Top_Axis_Hall.error[0], ENCOD_PLL.ADC_W, ENCOD_PLL.ADC_Z);
        // printf_DMA("ANGLE:%d,%d,%f,%f\n", Top_Axis_Hall.TOP_PLL_Theta, Top_Rotor_Hall.TOP_PLL_Theta, Top_Axis_Hall.n_m, Top_Rotor_Hall.n_m);
        printf_DMA("ANGLE:%d,%d,%d,%f\n", Top_Axis_Hall.TOP_PLL_Theta, theta_I, Theta_O, Top_Axis_Hall.amplitude);
        Pgy_MainLoop();
        						//   printf_DMA( "angle:%d,%d,%d\n",ENCOD_PLL.ADC_X,ENCOD_PLL.ADC_Y,ENCOD_PLL.ANGLE_ROTOR_MINT_Calib_E);

        //			printf_DMA( "angle:%f\n",ENCOD_PLL.SPEED_ROTOR_FILT);

        //				printf_DMA("ANGLE:%f,%f,%f\n", ENCOD_PLL.SPEED_ROTOR_FILT,currentM0.q,ENCOD_PLL.SPEED_ROTOR_S);
        //			printf_DMA("ANGLE:%d\n", Pa_PWM);
        //			printf_DMA( "angle:%d,%f,%f\n",SMO_Esti.SmoPLL.PLL_AngleOut,ENCOD_PLL.SPEED_ROTOR_FILT,currentM0.q);
        //			printf_DMA( "angle:%d,%d,%d,%d\n",ENCOD_PLL.ADC_W,ENCOD_PLL.ADC_X,ENCOD_PLL.ADC_Y,ENCOD_PLL.ADC_Z);

        //			printf_DMA( "angle:%f,%d,%f,%f,%d\n",M0.PID_vel.output,Pa_PWM,ENCOD_PLL.SPEED_ROTOR_S,currentM0.q,ENCOD_PLL.Angle_E_sum);
        //			printf_DMA( "angle:%f,%d,%f,%f\n",PowerOutput,Pa_PWM,ENCOD_PLL.SPEED_ROTOR_FILT,currentM0.q);

        //			printf_DMA("ANGLE:%f,%d,%f\n",position,ENCOD_PLL.ANGLE_ROTOR_MINT_Calib,PhasecurrentM0.a);
        //			printf("ANGLE:%d\n",ENCOD_PLL.ANGLE_ROTOR_MINT_Calib);

        //			if(position<180)
        //			{
        //				positionrec = position;
        //			}
        //			else
        //			{
        //				positionrec = position -360;
        //			}
        //			printf_DMA("ANGLE:%f,%f\n",M0.P_ang.ref,positionrec);

        //			printf_DMA("ANGLE:%f,%f\n",currentM0.q,M0.P_ang.output);
        //				printf("ANGLE:%d,%d\n",ENCOD_PLL.ANGLE_ROTOR_MINT,ENCOD_PLL.ANGLE_ROTOR_INT);
        //			printf("ANGLE:%f,%f\n",ENCOD_PLL.SPEED_ROTOR,ENCOD_PLL.SPEED_ROTOR_FILT);
        //			printf_DMA("ANGLE:%f,%f\n",ENCOD_PLL.SPEED_ROTOR_FILT,M0.PID_vel.ref);
        //			printf("ANGLE:%d,%d\n",M0.electrical_angle_int,ENCOD_PLL.ANGLE_ROTOR_INT);
        //			printf("ANGLE:%d,%d\n",ABZEncoder.AB_count_new,ABZEncoder.ANGLE_ROTOR_INT);
        //			printf("ANGLE:%d,%d\n",ABZEncoder.ANGLE_ROTOR_INT,M0.electrical_angle_int);
        //			printf("ANGLE:%d\n",20%(99));
        //			printf_DMA("ANGLE:%d,%d,%d\n",calibREC,EMFAngleTst.ANGLE_ROTOR_MINT_Calib,ENCOD_PLL.ANGLE_ROTOR_EINT_Calib);

        //    CiA402_DeallocateAxis();

        //    HW_Release();
        ////    return 0;
        //		commander_run();  //指令接收
    }
}

/**
 * @brief System Clock Configuration
 * @retval None
 */
void SystemClock_Config(void)
{
}

/* USER CODE BEGIN 4 */
void commander_run(void)
{
    if (rcv0_flag == 1) {
        rcv0_flag = 0;
        switch (rcv0_buff[0]) {
            case 'H':
                //				USART2_SendDMA(sprintf(snd2_buff,"Hello World!\r\n"));

                //				HAL_UART_Transmit_DMA(&huart1,"hello windows",13);
                printf("hello windows\r\n");
                break;

            case 'R':
                motor_status = Running;
                M0.enable    = 1;
                M1.enable    = 1;
                break;
            case 'Y':
                M0.enable = 1;
                M1.enable = 1;
                //	motor_status = PrePos;
                break;
            case 'V':
                SpeedRef = atof((const char *)(rcv0_buff + 1));
                break;

            case 'P':
                MosShut();
                break;
        }
        memset(rcv0_buff, 0, 16);

        //	HAL_UART_Receive_DMA(&huart1,rx_buffer,BUFFER_SIZE);
    }
}
/* USER CODE END 4 */

/**
 * @brief  This function is executed in case of error occurrence.
 * @retval None
 */
void Error_Handler(void)
{
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    while (1) {
    }
    /* USER CODE END Error_Handler_Debug */
}

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* USER CODE BEGIN 6 */
    /* User can add his own implementation to report the file name and line number,
       ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
    /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
