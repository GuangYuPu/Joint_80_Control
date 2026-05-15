
#ifndef _ADC_H
#define _ADC_H

#include "main.h"

/******************************************************************************/
extern  unsigned short adc1_value[32];
/******************************************************************************/
void ADC_Common_Init(void);
void ADC3_DMA_Init(void);
void ADC2_TRGO_Init(void);
void ADC1_TRGO_Init(void);
void BusVolSense(void);
void BoardTemSense(void);
void ILoopDrag_D(int16_t AngleAdd,float DragCur);
void StatorTemSense(void);
void ILoopPos(int16_t pos,float DragCur);

extern int16_t angle_compens;
extern int16_t AngleAdd ;
extern int16_t ANGLE ;
extern int16_t difftest ;
extern float SineWave ;
extern float SineWaveInt ;
extern uint8_t sensorlessFlag;
/******************************************************************************/


#endif


