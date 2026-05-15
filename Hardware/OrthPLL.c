#include "OrthPLL.h"
#include "main.h"
#include "stdlib.h"
#include "Protection.h"
#include "adc.h"
#include "FlashRW.h"

ENCOD_PLL_T ENCOD_PLL;

void OrthPLLInit(void)
{
//	ENCOD_PLL.PLL.P = 1000;//2000;
//	ENCOD_PLL.PLL.I = 40000;//80000;//80000;
	ENCOD_PLL.PLL.P = 10000;
	ENCOD_PLL.PLL.I = 240000;//80000;
	ENCOD_PLL.PLL.D = 0;
	
	ENCOD_PLL.PLL.error_prev = 0;
	ENCOD_PLL.PLL.integral_prev = 0;
	ENCOD_PLL.PLL.output_prev = 0;
	
	ENCOD_PLL.PLL.limit = 40000;
	ENCOD_PLL.PLL.output = 0;
	
	
	ENCOD_PLL.sineValue = 0;
	ENCOD_PLL.cosValue = 0;
}

void Orth_CAL(ENCOD_PLL_T * V)
{
	float encod1,encod2,error_encod,tempencod1,tempencod2,tempencod3;
	Trig_Components Local_Vector_ComponentsSMO;
	
//	V->ADC_X_1st = V->ADC_X_1st * 0.5 + V->ADC_X * 0.5;
//	V->ADC_Y_1st = V->ADC_Y_1st * 0.5 + V->ADC_Y * 0.5;
//	
//	V->ADC_X_2st = V->ADC_X_2st * 0.5 + V->ADC_X_1st * 0.5;
//	V->ADC_Y_2st = V->ADC_Y_2st * 0.5 + V->ADC_Y_1st * 0.5;
//	
//	encod1 = V->ADC_X_2st * V->ADC_X_2st;
//	encod2 = V->ADC_Y_2st * V->ADC_Y_2st;
	
//	tempencod1 = encod1 + encod2;
//	tempencod1 = sqrt(tempencod1);
	
//	tempencod2 = V->ADC_X_2st / tempencod1;
//	tempencod3 = V->ADC_Y_2st / tempencod1;

//	encod1 = V->ADC_X_1st * V->ADC_X_1st;
//	encod2 = V->ADC_Y_1st * V->ADC_Y_1st;

	encod1 = V->ADC_X * V->ADC_X;
	encod2 = V->ADC_Y * V->ADC_Y;
	
	tempencod1 = encod1 + encod2;
	tempencod1 = sqrt32(tempencod1);

	
	tempencod2 = V->ADC_X / tempencod1;
	tempencod3 = V->ADC_Y / tempencod1;

	encod1 =  tempencod2*ENCOD_PLL.cosValue;
	encod2 =  tempencod3*ENCOD_PLL.sineValue;
	
	error_encod = encod1 - encod2;
	
	PLL_Cal(&ENCOD_PLL.PLL,error_encod,Ts_I);
	
	Local_Vector_ComponentsSMO = MCM_Trig_Functions(ENCOD_PLL.ANGLE_ROTOR_MINT);
	ENCOD_PLL.sineValue = Local_Vector_ComponentsSMO.hSin;
	ENCOD_PLL.cosValue = Local_Vector_ComponentsSMO.hCos;
	
}
int16_t comps = 0;
void PLL_Cal(PIDController* PID,float error,float Tstep)
{
    float Ts;
    float proportional,integral,derivative,output;
    float output_rate;

		Ts = Tstep;

    // u(s) = (P + I/s + Ds)e(s)
    // Discrete implementations
    // proportional part
    // u_p  = P *e(k)
    proportional = PID->P * error;
    // Tustin transform of the integral part
    // u_ik = u_ik_1  + I*Ts/2*(ek + ek_1)
    integral = PID->integral_prev + PID->I*Ts*0.5f*(error + PID->error_prev);
    // antiwindup - limit the output
    integral = _constrain(integral, -PID->limit, PID->limit);
    // Discrete derivation
    // u_dk = D(ek - ek_1)/Ts
    derivative = PID->D*(error - PID->error_prev)/Ts;

    // sum all the components
    output = proportional + integral + derivative;
    // antiwindup - limit the output variable
    output = _constrain(output, -PID->limit, PID->limit);

    // if output ramp defined
    if(PID->output_ramp > 0)
    {
        // limit the acceleration by ramping the output
        output_rate = (output - PID->output_prev)/Ts;
        if(output_rate > PID->output_ramp)output = PID->output_prev + PID->output_ramp*Ts;
        else if(output_rate < -PID->output_ramp)output = PID->output_prev - PID->output_ramp*Ts;
    }


		ENCOD_PLL.SPEED_ROTOR = output; //单位为r/min

		
//		ENCOD_PLL.ANGLE_ROTOR_MINT1 += 0.5f*(output + PID->output_prev)*0.01092266666666f;
//		if(ENCOD_PLL.ANGLE_ROTOR_MINT1 > 0)
//		{
//		
//			ENCOD_PLL.ANGLE_ROTOR_MINT_Calib = ENCOD_PLL.ANGLE_ROTOR_MINT1;
//		}
//		else
//		{
//			ENCOD_PLL.ANGLE_ROTOR_MINT_Calib = 65536 + (ENCOD_PLL.ANGLE_ROTOR_MINT1);
//		}
		
		
		
		ENCOD_PLL.ANGLE_ROTOR_MINT += 0.5f*(output + PID->output_prev)*0.1092266666666f; //积分输出机械角度 V/60*6.28*65536/6.28*0.0001；
		
		if(ENCOD_PLL.ANGLE_ROTOR_MINT > 0)
		{
		
			ENCOD_PLL.ANGLE_ROTOR_MINT_Calib_E = ENCOD_PLL.ANGLE_ROTOR_MINT;

		}
		else
		{
			ENCOD_PLL.ANGLE_ROTOR_MINT_Calib_E = 65536 + (ENCOD_PLL.ANGLE_ROTOR_MINT);
		}
		
//		ENCOD_PLL.ANGLE_ROTOR_INT = ((ENCOD_PLL.ANGLE_ROTOR_MINT-OrthATOCALIB.EroMeanRec)*21); 
				ENCOD_PLL.ANGLE_ROTOR_INT = ((ENCOD_PLL.ANGLE_ROTOR_MINT_Calib_E - OrthATOCALIB.EroMeanRec)); 
		
			if(ENCOD_PLL.ANGLE_ROTOR_INT > 0)
		{

			ENCOD_PLL.ANGLE_ROTOR_EINT_Calib = ENCOD_PLL.ANGLE_ROTOR_MINT;

		}
		else
		{
			ENCOD_PLL.ANGLE_ROTOR_EINT_Calib = 65536 + (ENCOD_PLL.ANGLE_ROTOR_MINT);
		}

    // saving for the next pass
    PID->integral_prev = integral;
    PID->output_prev = output;
    PID->error_prev = error;
}

void PLLSpeedCal(void)
{
	ENCOD_PLL.vari_s = OrthATOCALIB.DirDetect *( ENCOD_PLL.ANGLE_ROTOR_MINT_Calib_E - ENCOD_PLL.ANGLE_ROTOR_MINT_Calib_OLD);
	if(abs(ENCOD_PLL.vari_s) > 32768)
	{
		if(ENCOD_PLL.vari_s>0)
		{
			ENCOD_PLL.vari_s = -(65536%ENCOD_PLL.vari_s);
		}
		else
		{
			ENCOD_PLL.vari_s = (65536%ENCOD_PLL.vari_s);
		}
	}
	ENCOD_PLL.SPEED_ROTOR_FILT = ENCOD_PLL.vari_s*0.0152587890625f * 0.1f  *20.0f;
	ENCOD_PLL.ANGLE_ROTOR_MINT_Calib_OLD = ENCOD_PLL.ANGLE_ROTOR_MINT_Calib_E;
}

void PLLSpeedCalEangle(void)
{
	static int16_t vari_sE;
	static uint16_t EangleIntOld;
	vari_sE = OrthATOCALIB.DirDetect *( ENCOD_PLL.ANGLE_ROTOR_EINT_Calib - EangleIntOld);
	if(abs(vari_sE) > 32768)
	{
		if(vari_sE>0)
		{
			vari_sE = -(65536%vari_sE);
		}
		else
		{
			vari_sE = (65536%vari_sE);
		}
	}
	ENCOD_PLL.SPEED_ROTOR_S = vari_sE*0.030517578f;
	EangleIntOld = ENCOD_PLL.ANGLE_ROTOR_EINT_Calib;
}


void EAngleSum(void)
{
//	static int32_t vari_Eangle;
//	static uint16_t overNum;
	ENCOD_PLL.vari_Eangle = ENCOD_PLL.ANGLE_ROTOR_EINT_Calib - ENCOD_PLL.EangleLast;
	
	if(ENCOD_PLL.vari_Eangle < -32768)
	{
		ENCOD_PLL.Angle_E_sum = ENCOD_PLL.Angle_E_sum + 65536+ENCOD_PLL.vari_Eangle;
	}
	else if(ENCOD_PLL.vari_Eangle > 32768)
	{
		ENCOD_PLL.Angle_E_sum = ENCOD_PLL.Angle_E_sum + 65536-ENCOD_PLL.vari_Eangle;
	}
	else
	{
			ENCOD_PLL.Angle_E_sum = ENCOD_PLL.Angle_E_sum + ENCOD_PLL.vari_Eangle;
	}
	
	ENCOD_PLL.EangleLast = ENCOD_PLL.ANGLE_ROTOR_EINT_Calib;
	
}



void EAngleSpeedCal(void)
{
	static float vari_sp1k;
	vari_sp1k = ENCOD_PLL.Angle_E_sum*0.1f;
	ENCOD_PLL.SPEED_ROTOR_S = vari_sp1k*0.01525f;
	ENCOD_PLL.Angle_E_sum = 0;
}





OrthATOCALIB_T OrthATOCALIB;

void DirDetCal(int16_t dirdata)
{
	static uint16_t posCount = 0,negCount = 0;
		
	if(dirdata>0)
	{
		OrthATOCALIB.DirPosCount++;
	}
	else if (dirdata<0)
	{
		OrthATOCALIB.DirNegCount++;
	}
	else
	{
	
	}
		
	
}


int16_t DirDiff = 0;
void OrthAUTOCalib(void)
{
	static uint16_t DirDtcOld = 0;

	
	if(OrthATOCALIB.DirDtc_FLAG == 0)
	{
		OrthATOCALIB.DirDtc_count++;
		if(OrthATOCALIB.DirDtc_count < 60000)
		{
				ILoopDrag_D(50,12);

			if((OrthATOCALIB.DirDtc_count%20) == 0) //将10k分频为1k
			{

				DirDiff = ENCOD_PLL.ANGLE_ROTOR_MINT_Calib_E - DirDtcOld;
				
					if(abs(DirDiff)>32768)
					{
							if(DirDiff > 0 )
							{
									DirDiff = - (65536 % DirDiff);
							}
							else
							{
									DirDiff =  (65536 % DirDiff);
							}
					}
				DirDetCal(DirDiff);
				DirDtcOld = ENCOD_PLL.ANGLE_ROTOR_MINT_Calib_E;
					
			}
		}
		else
		{
			OrthATOCALIB.DirDtc_FLAG = 1;
			if((OrthATOCALIB.DirPosCount-OrthATOCALIB.DirNegCount) > 100)
			{
				OrthATOCALIB.DirDetect = 1;
			}
			else if(((OrthATOCALIB.DirNegCount-OrthATOCALIB.DirPosCount) > 100))
			{
				OrthATOCALIB.DirDetect = -1;
			}
			else
			{
				MosShut();
			}
		}
	}
	else
	{
		OrthATOCALIB.Poles = 10;
//		OrthATOCALIB.DirDetect = -1;
		if(OrthATOCALIB.ATO_FLAG == 0&&OrthATOCALIB.ATO_Eturns == 0)
		{
			OrthZeroPosiSet(&M0,&M1);
//			PWMStandby();
		}
		if(OrthATOCALIB.ATO_FLAG == 0&&OrthATOCALIB.ATO_Eturns < OrthATOCALIB.Poles&&OrthATOCALIB.ATO_Eturns > 0)
		{
			OrthATOCALIB.ATO_count++;
			if(OrthATOCALIB.ATO_count < 8192)
			{
				ILoopDrag_D(8,8);  //2048*32 = 65536,强拉的话差不多一个周期
			}
			else
			{
				OrthZeroPosiSet(&M0,&M1);
				if(OrthATOCALIB.ATO_Eturns == OrthATOCALIB.Poles)
				{
					OrthATOCALIB.ATO_FLAG = 1;
					int32_t Ero_Sum = 0;

					for (uint8_t i = 0; i < OrthATOCALIB.Poles; i++)
					{
//						if(OrthATOCALIB.EroRec[i] > 1560)  //大于半个周期算作负方向，否则会出现累计的误差
//						{
//							OrthATOCALIB.EroRec[i] = OrthATOCALIB.EroRec[i] - 3121;
//						}
						Ero_Sum += OrthATOCALIB.EroRec[i];
					}
					OrthATOCALIB.EroMeanRec = Ero_Sum / OrthATOCALIB.Poles;
					
					write_to_flash(OrthATOCALIB.EroMeanRec,OrthATOCALIB.DirDetect);
//					PWMStandby();
					motor_status = Standby;
				}
			}
		}
	}
}

void OrthZeroPosiSet(MOTORController*M, MOTORController*N)
{
    static uint16_t prepos_flg = 0;

    prepos_flg++;

    if(prepos_flg <= 6000)
    {
//        setPhaseVoltage(&M0,0,3,0);
//        setPhaseVoltage(&M1,0,3,32768);
			
			ILoopPos(0,8);
    }
    else if(prepos_flg > 6000)
    {
			
			OrthATOCALIB.PosiRec[OrthATOCALIB.ATO_Eturns] = ENCOD_PLL.ANGLE_ROTOR_MINT_Calib_E;
			OrthATOCALIB.EroRec[OrthATOCALIB.ATO_Eturns] = (ENCOD_PLL.ANGLE_ROTOR_MINT_Calib_E);

			OrthATOCALIB.ATO_Eturns++;
			PWMStandby();
			OrthATOCALIB.ATO_count = 0;
			
			prepos_flg = 0;
    }
}


/***零电流反电势矫正***/
ENCOD_PLL_TST EMFAngleTst;

void OrthPLLInit_tst(void)
{
//	EMFAngleTst.PLL.P = 1000;//2000;
//	EMFAngleTst.PLL.I = 40000;//80000;//80000;
	EMFAngleTst.PLL_tst.P = 20000;
	EMFAngleTst.PLL_tst.I = 240000;//80000;
	EMFAngleTst.PLL_tst.D = 0;
	
	EMFAngleTst.PLL_tst.error_prev = 0;
	EMFAngleTst.PLL_tst.integral_prev = 0;
	EMFAngleTst.PLL_tst.output_prev = 0;
	
	EMFAngleTst.PLL_tst.limit = 30000;
	EMFAngleTst.PLL_tst.output = 0;
	
	EMFAngleTst.ADC_X_1st = 0;
	EMFAngleTst.ADC_Y_1st = 0;
	
	EMFAngleTst.sineValue = 0;
	EMFAngleTst.cosValue = 0;
}

float encod1_tst,encod2_tst,error_encod_tst,tempencod1_tst,tempencod2_tst,tempencod3_tst;
void Orth_CAL_f(ENCOD_PLL_TST * V)
{
//	static float encod1_tst,encod2_tst,error_encod_tst,tempencod1_tst,tempencod2_tst,tempencod3_tst;
	Trig_Components Local_Vector_ComponentsSMO_tst;
	
//	V->ADC_X_1st = V->ADC_X_1st * 0.5 + V->ADC_X * 0.5;
//	V->ADC_Y_1st = V->ADC_Y_1st * 0.5 + V->ADC_Y * 0.5;
//	
//	V->ADC_X_2st = V->ADC_X_2st * 0.5 + V->ADC_X_1st * 0.5;
//	V->ADC_Y_2st = V->ADC_Y_2st * 0.5 + V->ADC_Y_1st * 0.5;
//	
//	encod1 = V->ADC_X_2st * V->ADC_X_2st;
//	encod2 = V->ADC_Y_2st * V->ADC_Y_2st;
	
//	tempencod1 = encod1 + encod2;
//	tempencod1 = sqrt(tempencod1);
	
//	tempencod2 = V->ADC_X_2st / tempencod1;
//	tempencod3 = V->ADC_Y_2st / tempencod1;

//	encod1 = V->ADC_X_1st * V->ADC_X_1st;
//	encod2 = V->ADC_Y_1st * V->ADC_Y_1st;

	encod1_tst = V->ADC_X * V->ADC_X;
	encod2_tst = V->ADC_Y * V->ADC_Y;
	
	tempencod1_tst = encod1_tst + encod2_tst;
	tempencod1_tst = sqrt(tempencod1_tst);


	tempencod2_tst = V->ADC_X / tempencod1_tst;
	tempencod3_tst = V->ADC_Y / tempencod1_tst;

	encod1_tst =  tempencod2_tst*EMFAngleTst.cosValue;
	encod2_tst =  tempencod3_tst*EMFAngleTst.sineValue;

	error_encod_tst = encod1_tst - encod2_tst;

	PLL_Cal_TST(&EMFAngleTst.PLL_tst,error_encod_tst,Ts_I);
	
	Local_Vector_ComponentsSMO_tst = MCM_Trig_Functions(EMFAngleTst.ANGLE_ROTOR_MINT);
	EMFAngleTst.sineValue = Local_Vector_ComponentsSMO_tst.hSin;
	EMFAngleTst.cosValue = Local_Vector_ComponentsSMO_tst.hCos;
	
}

int32_t calibREC = 0;
int32_t IdIqZeroRec[21];
uint8_t ZeroRecCount = 0;
int16_t ZeroRecMean = 0;

void PLL_Cal_TST(PIDController* PID,float error,float Tstep)
{
    float Ts;
    float proportional_tst,integral_tst,derivative_tst,output_tst;
    float output_rate;

		Ts = Tstep;

    // u(s) = (P + I/s + Ds)e(s)
    // Discrete implementations
    // proportional part
    // u_p  = P *e(k)
    proportional_tst = PID->P * error;
    // Tustin transform of the integral part
    // u_ik = u_ik_1  + I*Ts/2*(ek + ek_1)
    integral_tst = PID->integral_prev + PID->I*Ts*0.5f*(error + PID->error_prev);
    // antiwindup - limit the output
    integral_tst = _constrain(integral_tst, -PID->limit, PID->limit);
    // Discrete derivation
    // u_dk = D(ek - ek_1)/Ts
    derivative_tst = PID->D*(error - PID->error_prev)/Ts;

    // sum all the components
    output_tst = proportional_tst + integral_tst + derivative_tst;
    // antiwindup - limit the output variable
    output_tst = _constrain(output_tst, -PID->limit, PID->limit);

    // if output ramp defined
    if(PID->output_ramp > 0)
    {
        // limit the acceleration by ramping the output
        output_rate = (output_tst - PID->output_prev)/Ts;
        if(output_rate > PID->output_ramp)output_tst = PID->output_prev + PID->output_ramp*Ts;
        else if(output_rate < -PID->output_ramp)output_tst = PID->output_prev - PID->output_ramp*Ts;
    }


		EMFAngleTst.SPEED_ROTOR = output_tst; //单位为r/min
		
		EMFAngleTst.ANGLE_ROTOR_MINT += 0.5f*(output_tst + PID->output_prev)*0.05461333333f; //积分输出机械角度 V/60*6.28*65536/6.28*0.0001；
		
		if(EMFAngleTst.ANGLE_ROTOR_MINT > 0)
		{
		
			EMFAngleTst.ANGLE_ROTOR_MINT_Calib = EMFAngleTst.ANGLE_ROTOR_MINT;

		}
		else
		{
			EMFAngleTst.ANGLE_ROTOR_MINT_Calib = 65536 + (EMFAngleTst.ANGLE_ROTOR_MINT);
		}
		
		EMFAngleTst.vari_s = EMFAngleTst.ANGLE_ROTOR_MINT_Calib - EMFAngleTst.ANGLE_ROTOR_MINT_Calib_OLD;
		if(EMFAngleTst.vari_s < -32768&&ENCOD_PLL.SPEED_ROTOR_FILT>2)
		{

			calibREC = ENCOD_PLL.ANGLE_ROTOR_EINT_Calib;
			
			IdIqZeroRec[ZeroRecCount] = calibREC*0.0476190476f;

			ZeroRecCount++;
			if(ZeroRecCount>20)
			{
				ZeroRecCount = 0;
				uint16_t tempsum = 0;
				for(uint8_t icount=0;icount<21;icount++)
				{
					tempsum +=IdIqZeroRec[icount];
				}
				ZeroRecMean = tempsum/21;
//				OrthATOCALIB.EroMeanRec = ZeroRecMean;
//				write_to_flash(OrthATOCALIB.EroMeanRec,OrthATOCALIB.DirDetect);
			}
		}
		
//		EMFAngleTst.ANGLE_ROTOR_INT = ((EMFAngleTst.ANGLE_ROTOR_MINT-OrthATOCALIB.EroMeanRec)*21); 
//	EMFAngleTst.ANGLE_ROTOR_INT = ((EMFAngleTst.ANGLE_ROTOR_MINT )*21); 
	EMFAngleTst.ANGLE_ROTOR_MINT_Calib_OLD = EMFAngleTst.ANGLE_ROTOR_MINT_Calib;
    // saving for the next pass
    PID->integral_prev = integral_tst;
    PID->output_prev = output_tst;
    PID->error_prev = error;
}