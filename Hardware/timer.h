
#ifndef _TIMER_H
#define _TIMER_H

#include "main.h"
/******************************************************************************/
//#define PWM_Period  3500
/******************************************************************************/
void TIM1_PWM_Init(void);
void TIM8_PWM_Init(void);
/******************************************************************************/
void TIM4_Init(void);
void TIM5_Init(void);
//float IC_GETFREQ(void);
//float IC_DUTYCYLE(void);
void IWDG_Init_Config(void);

extern float testfreq ;
extern float testdutycycle ;

#endif
