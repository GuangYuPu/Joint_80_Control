#include "FSM.h"

float HALL_Comp[6] = {0};

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

void FSM_AxisHallCalib()
{
    if (motor_status_last != motor_status) {
        calib_done = 0;
        for (uint16_t i = 0; i < COMP_TABLE_SIZE; i++) {
            calib_valid[i] = 0;
        }
    } else {
        TimGpioReInit();
        SpeedRef = 5;
    }
    M0.enable               = 1;
    M0.PID_d.ref            = 0;
    M0.electrical_angle_int = ENCOD_PLL.ANGLE_ROTOR_INT; // + ABZEncoder.InitPos;
    loopFOC(&M0);

    if (calib_done) {
        MosShut();
        SpeedRef     = 0;
        motor_status = WriteToFlash;
    }
}

void FSM_HallAdcCalib()
{
    TimGpioReInit();
    M0.enable    = 1;
    M0.PID_d.ref = 0;
    M0.PID_q.ref = 8;
    M0.electrical_angle_int += (int16_t)((I_Drag_Frq / (20e3)) * 65536); // + ABZEncoder.InitPos;

    loopFOC(&M0);

    global_time += (1 / 20e3);

    if (ADC1->JDR3 >= HallW_M[0]) HallW_M[0] = ADC1->JDR3;
    if (ADC1->JDR3 <= HallW_M[1]) HallW_M[1] = ADC1->JDR3;

    if (ADC1->JDR2 >= HallZ_M[0]) HallZ_M[0] = ADC1->JDR2;
    if (ADC1->JDR2 <= HallZ_M[1]) HallZ_M[1] = ADC1->JDR2;

    if (ADC1->JDR1 >= HallX_M[0]) HallX_M[0] = ADC1->JDR1;
    if (ADC1->JDR1 <= HallX_M[1]) HallX_M[1] = ADC1->JDR1;

    if (ADC2->JDR1 >= HallY_M[0]) HallY_M[0] = ADC2->JDR1;
    if (ADC2->JDR1 <= HallY_M[1]) HallY_M[1] = ADC2->JDR1;

    if ((global_time * I_Drag_Frq) >= (35 * 10)) {
        HALL_Comp[0] = (HallW_M[0] + HallW_M[1]) / 2;
        HALL_Comp[1] = (HallX_M[0] + HallX_M[1]) / 2;
        HALL_Comp[2] = (HallY_M[0] + HallY_M[1]) / 2;
        HALL_Comp[3] = (HallZ_M[0] + HallZ_M[1]) / 2;

        HALL_Comp[4] = (HallX_M[0] - HallX_M[1]) / (HallY_M[0] - HallY_M[1]);
        HALL_Comp[5] = (HallW_M[0] - HallW_M[1]) / (HallZ_M[0] - HallZ_M[1]);

        MosShut();
        motor_status = WriteToFlash;
    }
}