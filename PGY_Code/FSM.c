#include "FSM.h"

void FSM_SoftShut()
{
    SpeedRef = 0;
    PID_init(&M0);
}