#include "Estimate.h"


SMO_Observer SMO_Esti;


void Pos_Esti_Init(void)
{
	
	SMO_Esti.SmoPLL.Ref = 0;
	SMO_Esti.SmoPLL.Actual =0;
	SMO_Esti.SmoPLL.Err_now = 0;
	
	SMO_Esti.SmoPLL.Kp = 250;
	SMO_Esti.SmoPLL.Ki = 15;//锁相环pi
	
	SMO_Esti.SmoPLL.Pout = 0;
	SMO_Esti.SmoPLL.Iout = 0;
	SMO_Esti.SmoPLL.I_limit = 4000;
	SMO_Esti.SmoPLL.Out_limit = 4000;
	SMO_Esti.SmoPLL.Output = 0;
	
	SMO_Esti.SmoPLL.PLL_SpeedOut_now = 0;
	SMO_Esti.SmoPLL.PLL_SpeedOut_last = 0;
	SMO_Esti.SmoPLL.PLL_Speed = 0;
	SMO_Esti.SmoPLL.PLL_AngleOut = 0;
	SMO_Esti.SmoPLL.PLL_AngleOut_MINT = 0;
	
	
	SMO_Esti.CurrGain = 0.95f;//测试电机,1-(Rs*Ts/Ld)=1-(1.02*0.0001/0.59*0.001)=0.82712
	SMO_Esti.VoltGain = 0.25f;//Ts/Ld = 0.16949
	
	SMO_Esti.SgnGain = 8;
	
	SMO_Esti.preUalpha = 0;
	SMO_Esti.preUbeta = 0;
	SMO_Esti.preIalpha = 0;
	SMO_Esti.preIbeta = 0;
	
	SMO_Esti.EstiIalpha = 0;
	SMO_Esti.EstiIbeta = 0;
	
	SMO_Esti.BEMF_Alpha = 0;
	SMO_Esti.BEMF_Beta = 0;
	
	SMO_Esti.BEMF_1st_Alpha = 0;
  SMO_Esti.BEMF_1st_Beta = 0; 
	
	SMO_Esti.BEMF_SinValue = 0;
	SMO_Esti.BEMF_CosValue = 0;
	SMO_Esti.Angle_SinValue = 0;
	SMO_Esti.Angle_CosValue = 0;

}


void PLL_Calc(PLL *v)
{
  
	v->Pout = (v->Kp * v->Err_now);
  
	v->Iout += (v->Ki* v->Err_now);

	v->Iout = RangeLimit(-v->Out_limit, v->Iout, v->Out_limit);

	v->Output = v->Pout + (v->Iout);
  
	v->Output = RangeLimit(-v->Out_limit, v->Output, v->Out_limit);
	
	v->PLL_SpeedOut_now = v->Output;//单位r/min
	
	v->PLL_AngleOut_MINT += v->PLL_SpeedOut_now *0.05461333333f;
		if(v->PLL_AngleOut_MINT > 0)
		{
		
			v->PLL_AngleOut_MINTCalib = v->PLL_AngleOut_MINT;
		}
		else
		{
			v->PLL_AngleOut_MINTCalib = 65536 + (v->PLL_AngleOut_MINT);
		}
	
	v->PLL_AngleOut += v->PLL_SpeedOut_now *0.54637f;//1.365925234268f;// 1.09218621475f;//10kHz,(1/9.55)*0.0001*(65536/(2*pi))*4
}

	uint32_t sqtemp;
void Pos_Esti_Calc(void)//滑模观测器
{
	float temp1, temp2, temp3;

	Trig_Components Local_Vector_ComponentsSMO;
	
	temp1 = SMO_Esti.preUalpha - SMO_Esti.BEMF_1st_Alpha;
	temp2 = temp1 * SMO_Esti.VoltGain;//见初始化
	temp1 = SMO_Esti.preIalpha * SMO_Esti.CurrGain;
	SMO_Esti.EstiIalpha = temp1 + temp2;
	SMO_Esti.BEMF_Alpha = (SMO_Esti.EstiIalpha - AlphBetaCurM0.alph)*SMO_Esti.SgnGain;
	
	temp1 = SMO_Esti.preUbeta - SMO_Esti.BEMF_1st_Beta;
	temp2 = temp1 * SMO_Esti.VoltGain;
	temp1 = SMO_Esti.preIbeta * SMO_Esti.CurrGain;
	SMO_Esti.EstiIbeta = temp1 + temp2;
	SMO_Esti.BEMF_Beta = (SMO_Esti.EstiIbeta - AlphBetaCurM0.beta)*SMO_Esti.SgnGain;
	
	SMO_Esti.BEMF_Alpha = RangeLimit((-27), SMO_Esti.BEMF_Alpha, 27);
	SMO_Esti.BEMF_Beta = RangeLimit((-27), SMO_Esti.BEMF_Beta, 27);
	
	SMO_Esti.BEMF_1st_Alpha = 0.75f * SMO_Esti.BEMF_1st_Alpha + 0.25f * SMO_Esti.BEMF_Alpha;
	SMO_Esti.BEMF_1st_Beta = 0.75f * SMO_Esti.BEMF_1st_Beta + 0.25f * SMO_Esti.BEMF_Beta;
//	SMO_Esti.BEMF_2nd_Alpha = SMO_Esti.BEMF_2nd_Alpha*0.75 + 0.25*SMO_Esti.BEMF_1st_Alpha;//Q14
//	SMO_Esti.BEMF_2nd_Beta = SMO_Esti.BEMF_2nd_Beta* 0.75 + 0.25*SMO_Esti.BEMF_1st_Beta;//Q14
	
	temp1 = SMO_Esti.BEMF_1st_Alpha;
	temp2 = SMO_Esti.BEMF_1st_Beta;
	
	temp3 = temp1*temp1 + temp2*temp2;
//	sqtemp = (int)(temp1*temp1 + temp2*temp2)*1048576;
	
	temp1 = sqrt(temp3);
//	temp1 =(float) sqrt32(sqtemp)*0.0009765625f;
	
	
	SMO_Esti.BEMF_SinValue = SMO_Esti.BEMF_1st_Alpha/temp1;
	SMO_Esti.BEMF_CosValue = SMO_Esti.BEMF_1st_Beta/temp1;
	
	temp1 = -SMO_Esti.BEMF_SinValue*SMO_Esti.Angle_CosValue;
	temp2 = SMO_Esti.BEMF_CosValue*SMO_Esti.Angle_SinValue;
	
	SMO_Esti.SmoPLL.Err_now = (temp1 - temp2);
	
	PLL_Calc(&SMO_Esti.SmoPLL);
	
	Local_Vector_ComponentsSMO = MCM_Trig_Functions(SMO_Esti.SmoPLL.PLL_AngleOut);
	SMO_Esti.Angle_SinValue = Local_Vector_ComponentsSMO.hSin;
	SMO_Esti.Angle_CosValue = Local_Vector_ComponentsSMO.hCos;

	SMO_Esti.preUalpha = AlphBetaVolM0.alph * voltage_power_supply/_SQRT3 ;
	SMO_Esti.preUbeta = AlphBetaVolM0.beta *  voltage_power_supply/_SQRT3;
	SMO_Esti.preIalpha = AlphBetaCurM0.alph;
	SMO_Esti.preIbeta = AlphBetaCurM0.beta;

}





void Anglee_Diff(void)
{
	
//	if( SMO_Esti.SmoPLL.PLL_AngleOut - ENCODER.ANGLE_ROTOR_INT >= 16384)
//			{
//				SMO_Esti.Diffree = ENCODER.ANGLE_ROTOR_INT - SMO_Esti.SmoPLL.PLL_AngleOut + 32768;
//			}
//	else if(ENCODER.ANGLE_ROTOR_INT - SMO_Esti.SmoPLL.PLL_AngleOut >= 16384)
//			{
//				SMO_Esti.Diffree = ENCODER.ANGLE_ROTOR_INT - SMO_Esti.SmoPLL.PLL_AngleOut - 32768;
//			}
//	else
//	    {

//			}
				SMO_Esti.Diffree = ENCOD_PLL.ANGLE_ROTOR_INT - SMO_Esti.SmoPLL.PLL_AngleOut;
			if(abs(SMO_Esti.Diffree)>32768)
			{
				if(SMO_Esti.Diffree>0)
				{
					SMO_Esti.Diffree = -(65536%SMO_Esti.Diffree);
				}
				else
				{
					SMO_Esti.Diffree = (65536%SMO_Esti.Diffree);
				}
			}
}

