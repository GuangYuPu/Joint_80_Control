#include "BLDCMotor.h"

#include "ABZ.h"
#include "timer.h"
#include "OrthPLL.h"
#include "Protection.h"
DQVoltage_s voltage;
DQCurrent_s current;

float PowerOutput = 0;
float Ua,Ub,Uc = 0;

uint16_t Pa_PWM = 0;
uint16_t Pb_PWM = 0;
uint16_t Pc_PWM = 0;



/******************************************************************************/


void loopFOC(MOTORController *M)
{
    if(M->enable==0)return;   //没有使能，直接退出，减少资源占用


//      current = getFOCCurrents(M,M->electrical_angle);
    current = getFOCCurrents(M,M->electrical_angle_int);
    // dq轴反馈电流滤波
//					current.q = LPFoperator(&M->LPF_q,current.q);
//					current.d = LPFoperator(&M->LPF_d,current.d);
    // calculate the phase voltages
    voltage.q = PIDoperator(&M->PID_q, (M->PID_q.ref-current.q),Ts_I);
    voltage.d = PIDoperator(&M->PID_d, (M->PID_d.ref-current.d),Ts_I);

    setPhaseVoltage(M, voltage.q, voltage.d, M->electrical_angle_int);
}
/******************************************************************************/
/******************************************************************************/
float test = 0.0f;
void setPhaseVoltage(MOTORController *M,float Uq, float Ud, float angle_el)
{
	static float temp = 0;
	static float ualph,ubeta,u1,u2,u3 = 0;
	static uint8_t sector = 0 ;
	static float Tx,Ty,T0,ta,tb,tc = 0.0f;
	
	Trig_Components Local_Vector_Components;
	Local_Vector_Components = MCM_Trig_Functions(angle_el);
	
	float cosAngle = Local_Vector_Components.hCos;
	float sinAngle = Local_Vector_Components.hSin;
	
	Ud = _SQRT3*Ud/voltage_power_supply;
	Uq = _SQRT3*Uq/voltage_power_supply;
//	if((Ud * Ud+Uq*Uq)>voltage_power_supply*voltage_power_supply)
//	{}
//	Ud = Ud/voltage_power_supply;
//	Uq = Uq/voltage_power_supply;
	
//	PowerOutput = 1.22474f*Ud * currentM0.d + 1.22474f*Uq * currentM0.q;

	ualph = Ud * cosAngle - Uq * sinAngle;
	ubeta = Ud * sinAngle + Uq * cosAngle;
	
//	Ua = ualph;
//	Ub = -0.5f*ualph + 0.866f*ubeta;
//	Uc = -0.5f*ualph - 0.866f*ubeta;
//	
//	PowerOutput = Ua*PhasecurrentM0.a-Ub*(PhasecurrentM0.a+PhasecurrentM0.c)+Uc*PhasecurrentM0.c;
	
	
	u1 = ubeta;
	u2 = (0.866025f * ualph) -  0.5f*ubeta;
	u3 = (-0.866025f * ualph) -  0.5f*ubeta;
	
	
	
	sector = (u1 > 0.0f) + ((u2 > 0.0f)<<1) + ((u3 > 0.0f)<<2);

	switch(sector)
	{
		case 3:
		{
			
			Tx = u2;
			Ty = u1;
			T0 = (1 - Tx -Ty)*0.5f;
			
			if (T0<0)
			{
				temp=1/(Tx+Ty);
				Tx=Tx*temp;
				Ty=Ty*temp;
				T0 = 0;
			}

			ta = Tx + Ty + T0;
			tb = Ty +T0;
			tc = T0;
			break;
		}
		case 1:
		{
			
			Tx = -(u2);
			Ty = -(u3);
			T0 = (1 - Tx -Ty)*0.5f;
			
			if (T0<0)
			{
				temp=1/(Tx+Ty);
				Tx=Tx*temp;
				Ty=Ty*temp;
				T0 = 0;
			}
			
			ta = Ty + T0;
			tb = Tx + Ty +T0;
			tc = T0;
			break;
		}
		case 5:
		{
			
			Tx = u1;
			Ty = u3;
			T0 = (1 - Tx -Ty)*0.5f;
			
			if (T0<0)
			{
				temp=1/(Tx+Ty);
				Tx=Tx*temp;
				Ty=Ty*temp;
				T0 = 0;
			}
			
			ta = T0;
			tb = Tx + Ty +T0;
			tc = Ty + T0;
			break;
		}
		case 4:
		{
			
			Tx = -(u1);
			Ty = -(u2);
			T0 = (1 - Tx -Ty)*0.5f;	

			if (T0<0)
			{
				temp=1/(Tx+Ty);
				Tx=Tx*temp;
				Ty=Ty*temp;
				T0 = 0;
			}
			
			ta = T0;
			tb = Ty +T0;
			tc = Tx + Ty + T0;
			break;
		}
		case 6:
		{
			
			Tx = u3;
			Ty = u2;
			T0 = (1 - Tx -Ty)*0.5f;
			
			if (T0<0)
			{
				temp=1/(Tx+Ty);
				Tx=Tx*temp;
				Ty=Ty*temp;
				T0 = 0;
			}

			ta = Ty + T0;
			tb = T0;
			tc = Tx + Ty + T0;
			break;
		}
		case 2:
		{
			
			Tx = -(u3);
			Ty = -(u1);
			T0 = (1 - Tx -Ty)*0.5f;
			
			if (T0<0)
			{
				temp=1/(Tx+Ty);
				Tx=Tx*temp;
				Ty=Ty*temp;
				T0 = 0;
			}
			
			ta = Tx + Ty + T0;
			tb = T0;
			tc = Ty + T0;
			break;
		}
	}

		AlphBetaVolM0.alph = ualph;
		AlphBetaVolM0.beta = ubeta;
		
		Pa_PWM = ta*PWM_Period;
		Pb_PWM = tb*PWM_Period;
		Pc_PWM = tc*PWM_Period;
		
		TIM1->CCR1 = ta*PWM_Period;
		TIM1->CCR2 = tb*PWM_Period;
		TIM1->CCR3 = tc*PWM_Period;

		
	
}
/******************************************************************************/


void motorPrepos(MOTORController*M, MOTORController*N)
{
    static uint16_t prepos_flg = 0;

    prepos_flg++;
    if(prepos_flg <500)
    {
        M->PID_d.ref = IFCurrent;
        M->PID_q.ref = 0.0f;

        N->PID_d.ref = IFCurrent;
        N->PID_q.ref = 0.0f;


        M0.electrical_angle_int = 16384;
				M1.electrical_angle_int = M0.electrical_angle_int + 32678;
        loopFOC(&M0);
        loopFOC(&M1);
    }
    else if(prepos_flg >= 500 && prepos_flg <= 1500)
    {
        M->PID_d.ref = IFCurrent;
        M->PID_q.ref = 0.0f;

        N->PID_d.ref = IFCurrent;
        N->PID_q.ref = 0.0f;


        M0.electrical_angle_int = 0;
				M1.electrical_angle_int = M0.electrical_angle_int + 32678;
        loopFOC(&M0);
        loopFOC(&M1);
    }
    else if(prepos_flg > 1000)
    {
//        M->PID_d.ref = 0.0f;
//        M->PID_q.ref = CurrentLoopAngleAddInit;

//        N->PID_d.ref = 0.0f;
//        N->PID_q.ref = CurrentLoopAngleAddInit;

				MosShut();
	
//        motor_status = Running;
    }

}
/****电压预定位***/


void VolMotorPrepos(MOTORController*M, MOTORController*N)
{
    static uint16_t prepos_flg = 0;

    

    if(prepos_flg <= 6000)
    {
			prepos_flg++;
        setPhaseVoltage(&M0,0,2,0);
//        setPhaseVoltage(&M1,0,1.5,32768);
//			testdutycycle = IC_DUTYCYLE();
//			PWMInitPosCal();
//			Orth_CAL(&ENCOD_PLL);
    }
    else if(prepos_flg > 6000)
    {
//			testdutycycle = IC_DUTYCYLE();
//			PWMInitPosCal();
//			ABZInit();
//			Orth_CAL(&ENCOD_PLL);
			PWMStandby();
			
//        setPhaseVoltage(&M0,0,1.5,0);
//        setPhaseVoltage(&M1,0,1,32768);

//			ATOCALIB.ATO_FLAG = 1;
        motor_status = Standby;
    }
}

/******************************************************************************/


