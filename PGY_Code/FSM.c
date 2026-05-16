#include "FSM.h"


float HALL_Comp[6] = {0};
// float HallW_Bias = 0;
// float HallZ_Bias = 0;
// float HallX_Bias = 0;
// float HallY_Bias = 0;
// float Gian_W = 0;
// float Gain_Y = 0;

float I_Drag_Frq = 50;

float HallW_M[2] = {2048, 2048};
float HallZ_M[2] = {2048, 2048};
float HallX_M[2] = {2048, 2048};
float HallY_M[2] = {2048, 2048};

void FSM_SoftShut()
{
    SpeedRef = 0;
    PID_init(&M0);
}