#include "foc_utils.h"
#include "MotorInit.h"
#include "lowsideCurrentsense.h"
#include "pid.h"
#include "OrthPLL.h"
void MotorInit(void)
{


    M0.id = 0;
    M0.IProt = 60.0;
    M0.enable = 0;  //电机绕组使能 0 为不使能，1为使能
    M0.controller = Type_velocity;

		M0.offset_ia = 2050;//initial offset
		M0.offset_ib = 2050;//initial offset
		M0.offset_ic = 2050;//initial offset

    LowsideCurrentSense(&M0,0.002,20,ADC_Channel_4,NOT_SET,ADC_Channel_6);

    PID_init(&M0);
		OrthPLLInit();
		OrthPLLInit_tst();
}
