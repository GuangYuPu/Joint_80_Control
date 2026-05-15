
#include "usart.h"
#include "stdlib.h"
#include "math.h"
#include "MyProject.h"
ENCODER_T ENCODER;







void ENCODER_Get (void)
{

	I2C_DMA_Read(SLAVE_ADDR, ANGLE_REGISTER_HIGH_ADDR,ENCODER.buf, (u8*)(sizeof(ENCODER.buf)));

	ENCODER.ANGLE_RAW = (int16_t)(ENCODER.buf[0]<<8)|(ENCODER.buf[1]);

	/**提前设定补偿**/

	/**第一组关节电机**/
//	ENCODER.ANGLE_Calib = ENCODER.ANGLE_RAW - 62 ;//1号机
//ENCODER.ANGLE_Calib = ENCODER.ANGLE_RAW + 24;//2号机

	
	/**第二组关节电机**/
//	ENCODER.ANGLE_Calib = ENCODER.ANGLE_RAW + 17 ;//1号机
ENCODER.ANGLE_Calib = ENCODER.ANGLE_RAW - 53;//2号机
//ENCODER.ANGLE_Calib = ENCODER.ANGLE_RAW + 30;//3号机
	
	/**第三组关节电机**/
//	ENCODER.ANGLE_Calib = ENCODER.ANGLE_RAW - 43;//1号机
//ENCODER.ANGLE_Calib = ENCODER.ANGLE_RAW - 90;//2号机
//ENCODER.ANGLE_Calib = ENCODER.ANGLE_RAW + 55;//3号机
/*****角度反向矫正****/
	ENCODER.ANGLE_Calib = -(ENCODER.ANGLE_Calib-4096) ;
	ENCODER.ANGLE_Calib = ENCODER.ANGLE_Calib % 4096;
	if(ENCODER.ANGLE_Calib < 0)
	{
		ENCODER.ANGLE_Calib = ENCODER.ANGLE_Calib + 4096;
	}
		/**提前设定补偿**/

	dirJudge();


	ENCODER.ANGLE_RAW_Poles = (ENCODER.ANGLE_Calib*21)%4096;

	ENCODER.ANGLE_ROTOR = (float) ENCODER.ANGLE_RAW_Poles*0.00153398078788f ;
	ENCODER.ANGLE_ROTOR_INT = ENCODER.ANGLE_ROTOR*10430.3783504704f;
	

}

void speedcapt(void)
{
    ENCODER.vari = ENCODER.ANGLE_Calib - ENCODER.angle_old;
    if(abs(ENCODER.vari)>2048)
    {
        if(ENCODER.vari > 0 )
        {
            ENCODER.vari = - (4096 % ENCODER.vari);
        }
        else
        {
            ENCODER.vari =  (4096 % ENCODER.vari);
        }
    }
    ENCODER.SPEED_ROTOR = (float)ENCODER.vari*0.244140625f;
    ENCODER.angle_old = ENCODER.ANGLE_Calib;

}






void dirJudge(void)
{

	if(ENCODER.vari > 0 || ENCODER.vari < -2048)
	{
		ENCODER.direction = 0;
	}
	
	if(ENCODER.vari < 0 || ENCODER.vari > 2048)
	{
		ENCODER.direction = 1;
	}

		if(ENCODER.vari > 2048)
		{
			ENCODER.turns--;
		}
		if(ENCODER.vari < -2048)
		{
			ENCODER.turns++;
		}
		
}
/**通过减速器计算端部位置**/
void RollerPosit(void)
{
	if(ENCODER.direction == 0)
	{
	ENCODER.ANGLE_Rducer_RAW = abs(ENCODER.turns) * 1024 + ENCODER.ANGLE_Calib/4;
	ENCODER.ANGLE_Rducer_RAW = ENCODER.ANGLE_Rducer_RAW % 4096;
	ENCODER.ANGLE_ROTOR_Rdu =(float) ENCODER.ANGLE_Rducer_RAW * 0.08789f;
	}
	if(ENCODER.direction == 1)
	{
		ENCODER.ANGLE_Rducer_RAW = -(ENCODER.ANGLE_Calib-4096);
		ENCODER.ANGLE_Rducer_RAW = abs(ENCODER.turns) * 1024 + ENCODER.ANGLE_Rducer_RAW/4;
		ENCODER.ANGLE_Rducer_RAW = ENCODER.ANGLE_Rducer_RAW % 4096;
		ENCODER.ANGLE_Rducer_RAW = -(ENCODER.ANGLE_Rducer_RAW-4096);

		ENCODER.ANGLE_ROTOR_Rdu =(float) ENCODER.ANGLE_Rducer_RAW * 0.08789f;
	}
	
}

void Climbing(MOTORController* M, float Vref)
{
	//1K频率速度环爬坡，斜率给8r/s/s,每次进1k的速度环加0.008指令,从0加速到8r/s需要1s
	if( fabs(M->PID_vel.ref - Vref) < 0.08 )
	{
		M->PID_vel.ref = Vref;
	}
	else if (M->PID_vel.ref < Vref)
	{
		M->PID_vel.ref += 0.08;
	}
	else if (M->PID_vel.ref > Vref)
	{
		M->PID_vel.ref -=0.08;
	}
}

void SpeedLimitClimb(MOTORController*M, MOTORController*N)
{
          	/**48v母线电压限幅**/
	
//			if(fabs(M->PID_vel.ref) <= 2)
//			{
//				M->PID_vel.limit = 5;
//				N->PID_vel.limit = 5;

//				M->PID_d.limit = 8;
//				M->PID_q.limit = 8;

//				N->PID_d.limit = 8;
//				N->PID_q.limit = 8;
//			}
//			else if(fabs(M->PID_vel.ref) <= 5)
//			{
//				M->PID_vel.limit = 8;
//				N->PID_vel.limit = 8;
//				
//				M->PID_d.limit = 12;
//				M->PID_q.limit = 12;

//				N->PID_d.limit = 12;
//				N->PID_q.limit = 12;
//			}
//			else if (fabs(M->PID_vel.ref) <=10)
//			{
//				M->PID_vel.limit = 8;
//				N->PID_vel.limit = 8;
//				
//				M->PID_d.limit = 20;
//				M->PID_q.limit = 20;

//				N->PID_d.limit = 20;
//				N->PID_q.limit = 20;
//			}
//			else if (fabs(M->PID_vel.ref) <=15)
//			{
//				M->PID_vel.limit = 8;
//				N->PID_vel.limit = 8;
//				
//				M->PID_d.limit = 40;
//				M->PID_q.limit = 40;

//				N->PID_d.limit = 40;
//				N->PID_q.limit = 40;
//			}
//			else if (fabs(M->PID_vel.ref)<=20)
//			{
//				M->PID_vel.limit = 10;
//				N->PID_vel.limit = 10;
//				
//				M->PID_d.limit = 40;
//				M->PID_q.limit = 40;

//				N->PID_d.limit = 40;
//				N->PID_q.limit = 40;
//			}
//			else if (fabs(M->PID_vel.ref)<=25)
//			{
//				M->PID_vel.limit = 12;
//				N->PID_vel.limit = 12;

//				M->PID_d.limit = 40;
//				M->PID_q.limit = 40;

//				N->PID_d.limit = 40;
//				N->PID_q.limit = 40;
//			}


					/**96v母线电压限幅**/
					
			if(fabs(M0.PID_vel.ref) <= 2)
			{
				M0.PID_vel.limit = 5;
				M1.PID_vel.limit = 5;

				M0.PID_d.limit = 10;
				M0.PID_q.limit = 10;

				M1.PID_d.limit = 10;
				M1.PID_q.limit = 10;
				
			}
			else if(fabs(M0.PID_vel.ref) <= 5)
			{
				M0.PID_vel.limit = 8;
				M1.PID_vel.limit = 8;
				
				M0.PID_d.limit = 20;
				M0.PID_q.limit = 20;

				M1.PID_d.limit = 20;
				M1.PID_q.limit = 20;
			}
			else if (fabs(M0.PID_vel.ref) <=10)
			{
				M0.PID_vel.limit = 12;
				M1.PID_vel.limit =12;
				
				M0.PID_d.limit = 20;
				M0.PID_q.limit = 20;

				M1.PID_d.limit = 20;
				M1.PID_q.limit = 20;
			}
			else if (fabs(M0.PID_vel.ref) <=15)
			{
				M0.PID_vel.limit = 12;
				M1.PID_vel.limit = 12;

				M0.PID_d.limit = 60;
				M0.PID_q.limit = 60;

				M1.PID_d.limit = 60;
				M1.PID_q.limit = 60;
			}
			else if (fabs(M0.PID_vel.ref)<=20)
			{
				M0.PID_vel.limit = 10;
				M1.PID_vel.limit = 10;

				M0.PID_d.limit = 60;
				M0.PID_q.limit = 60;

				M1.PID_d.limit = 60;
				M1.PID_q.limit = 60;
			}
			else if (fabs(M0.PID_vel.ref)<=25)
			{
				M0.PID_vel.limit = 12;
				M1.PID_vel.limit = 12;

				M0.PID_d.limit = 60;
				M0.PID_q.limit = 60;

				M1.PID_d.limit = 60;
				M1.PID_q.limit = 60;
			}
			else
			{
				M0.PID_vel.limit = 12;
				M1.PID_vel.limit = 12;

				M0.PID_d.limit = 60;
				M0.PID_q.limit = 60;

				M1.PID_d.limit = 60;
				M1.PID_q.limit = 60;
			}
}

int8_t posReversFlag = 0;

void Climbing_POS(MOTORController* M, float PosRef)
{
	static float posOld = 12;
	
	if(posReversFlag == 1)
	{
		posReversFlag = 0;
		posOld = -posOld;
	}
	else
	{
		if( fabs(M->P_ang.ref - posOld) < 0.08 )
		{
			M->P_ang.ref = posOld;
			posReversFlag = 1;
		}
		if (M->P_ang.ref < posOld)
		{
			M->P_ang.ref += 0.08;
		}
		else if (M->P_ang.ref > posOld)
		{
			M->P_ang.ref -=0.08;
		}
	}
}