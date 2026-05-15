#ifndef _TOP_PLL_H__
#define _TOP_PLL_H__

#include "UserInc.h"

#define TPI_PGY 6.283185307179586f

#define COMP_TABLE_SIZE 360
#define INT16_MIN (-32768)
#define INT16_MAX 32767
#define RANGE 65536L  // 16位有符号范围跨度

typedef struct
{
    float kp;
    float ki;
    float kd;
    float limit;
    float limitwe;
    
    float error[2];
    float accelarate;
    float we;
    float theta_e;
    float amplitude;
    
    float n_e;
    float n_m;
    int16_t TOP_PLL_Theta;
} Top_Pll_t;

extern Top_Pll_t Top_Rotor_Hall;
extern Top_Pll_t Top_Axis_Hall;

extern int16_t comp_I[COMP_TABLE_SIZE];
extern uint8_t calib_valid[COMP_TABLE_SIZE];
extern uint8_t calib_done;

void TOP_PLL_Init(Top_Pll_t *Top_Hall);
void TOP_PLL_Loop(Top_Pll_t *Top_Hall, float alfa, float beta, float Ts, float np);
void NOTOP_PLL_Loop(Top_Pll_t *Top_Hall, float alfa, float beta, float Ts, float np, float delta);

void Axis_Pos_Comp_Loop(void);

#endif