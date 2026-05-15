#include "Usermain.h"

void Pgy_Init()
{
    // User_Communication_Init();
    TOP_PLL_Init(&Top_Rotor_Hall);
    TOP_PLL_Init(&Top_Axis_Hall);
}

void Pgy_Fast_Loop()
{
    // global_time += 5e-5;

    // User_Communication_Loop();
    TOP_PLL_Loop(&Top_Rotor_Hall, (float)ENCOD_PLL.ADC_X, (float)ENCOD_PLL.ADC_Y, 5e-5, 10);
    NOTOP_PLL_Loop(&Top_Axis_Hall, (float)ENCOD_PLL.ADC_Z, (float)ENCOD_PLL.ADC_W, 5e-5, 1, 3.1415926f/4);
}

void Pgy_Slow_Loop()
{
    // global_time += 1e-3;

    Axis_Pos_Comp_Loop();
}
