#include "Usermain.h"

void Pgy_Init()
{
    // User_Communication_Init();
    TOP_PLL_Init(&Top_Rotor_Hall);
    TOP_PLL_Init(&Top_Axis_Hall);
    ReadFromFlash_PGY(comp_I, COMP_TABLE_SIZE, PGY_ADDR_FLASH_SECTOR_5);
    ReadFromFlash_PGY(HALL_Comp, 12, PGY_ADDR_FLASH_SECTOR_4);
}

void Pgy_Fast_Loop()
{
    // global_time += 5e-5;

    TOP_PLL_Loop(&Top_Rotor_Hall, (float)ENCOD_PLL.ADC_X, (float)ENCOD_PLL.ADC_Y, 5e-5, 10);
    NOTOP_PLL_Loop(&Top_Axis_Hall, (float)ENCOD_PLL.ADC_Z, (float)ENCOD_PLL.ADC_W, 5e-5, 1, 3.1415926f / 4);
}

void Pgy_Slow_Loop()
{
    // global_time += 1e-3;
    // User_Communication_Loop();
    Axis_Pos_Comp_Loop(Top_Rotor_Hall.we, 350, Top_Axis_Hall.TOP_PLL_Theta, 1e-3);
}

void Pgy_MainLoop()
{
    if (WriteToFlash_FLAG) {
        WriteToFlash_PGY(comp_I, COMP_TABLE_SIZE, PGY_ADDR_FLASH_SECTOR_5);
        WriteToFlash_PGY(HALL_Comp, 12, PGY_ADDR_FLASH_SECTOR_4);
        WriteToFlash_FLAG = 0;
    }
}
