
#include "MyProject.h"
#include "ABZ.h"
#include "OrthPLL.h"
#include "Estimate.h"

#include "UserInc.h"
/****************************************************************************/
uint16_t adc0_value[12];
/****************************************************************************/
// PC0/Channel_10==M0_SO1==注入转换T1_TRGO，PC3/Channel_13==M1_SO1==规则转换T8_TRGO，
// PC1/Channel_11==M0_SO2==注入转换T1_TRGO，PC2/Channel_12==M1_SO2==规则转换T8_TRGO，
void ADC_Common_Init(void)
{

    GPIO_InitTypeDef GPIO_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    ADC_CommonInitTypeDef ADC_CommonInitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2; ///*TEMP板子温度采样*/
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

    // M0是ABC  M1是UVW  采集AB相和UV相电流

    GPIO_InitStructure.GPIO_Pin  = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_5 | GPIO_Pin_6; // PC5-M0_temp
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ADC Common通用配置，只配置一次适用三个ADC外设

    //    NVIC_InitStructure.NVIC_IRQChannel = ADC_IRQn;
    ////	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
    ////	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    //    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    //    NVIC_Init(&NVIC_InitStructure);
    NVIC_SetPriority(ADC_IRQn, 1);
    NVIC_EnableIRQ(ADC_IRQn);

    // ADC Common通用配置，只配置一次适用三个ADC外设
    ADC_CommonInitStructure.ADC_Mode             = ADC_Mode_Independent;         // 独立模式
    ADC_CommonInitStructure.ADC_Prescaler        = ADC_Prescaler_Div4;           // 4分配21MHz
    ADC_CommonInitStructure.ADC_DMAAccessMode    = ADC_DMAAccessMode_Disabled;   // DMA禁止选项(对于多个ADC模式)
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles; // 采样延时
    ADC_CommonInit(&ADC_CommonInitStructure);
}
/****************************************************************************/
void ADC3_DMA_Init(void)
{

    NVIC_InitTypeDef NVIC_InitStructure;
    DMA_InitTypeDef DMA_InitStructure;
    ADC_InitTypeDef ADC_InitStructure;

    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC3, ENABLE);

    // DMA2_Stream0 Channel_0 对应ADC1
    DMA_InitStructure.DMA_Channel            = DMA_Channel_2;
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)&(ADC3->DR);           // ADC地址
    DMA_InitStructure.DMA_Memory0BaseAddr    = (uint32_t)&adc0_value;           // 内存地址
    DMA_InitStructure.DMA_DIR                = DMA_DIR_PeripheralToMemory;      // 外设到内存
    DMA_InitStructure.DMA_BufferSize         = 12;                              // 传输长度
    DMA_InitStructure.DMA_PeripheralInc      = DMA_PeripheralInc_Disable;       // 外设递增关闭
    DMA_InitStructure.DMA_MemoryInc          = DMA_MemoryInc_Enable;            // 内存递增开启
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord; // 数据宽度16bit
    DMA_InitStructure.DMA_MemoryDataSize     = DMA_MemoryDataSize_HalfWord;
    DMA_InitStructure.DMA_Mode               = DMA_Mode_Circular;    // 循环模式
    DMA_InitStructure.DMA_Priority           = DMA_Priority_Low;     // 优先级高
    DMA_InitStructure.DMA_FIFOMode           = DMA_FIFOMode_Disable; // DMA FIFO 关闭
    DMA_InitStructure.DMA_FIFOThreshold      = DMA_FIFOThreshold_HalfFull;
    DMA_InitStructure.DMA_MemoryBurst        = DMA_MemoryBurst_Single;
    DMA_InitStructure.DMA_PeripheralBurst    = DMA_PeripheralBurst_Single;
    DMA_Init(DMA2_Stream1, &DMA_InitStructure);
    // 使能DMA

    //    NVIC_InitStructure.NVIC_IRQChannel=DMA2_Stream1_IRQn;   //
    //		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 6;
    //		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    //    NVIC_InitStructure.NVIC_IRQChannelCmd=ENABLE;
    //    NVIC_Init(&NVIC_InitStructure);
    ////		NVIC_SetPriority (DMA2_Stream1_IRQn, 1);
    ////		NVIC_EnableIRQ(DMA2_Stream1_IRQn);
    //
    //    DMA_ClearFlag(DMA2_Stream1, DMA_IT_TC);
    //    DMA_ITConfig(DMA2_Stream1, DMA_IT_TC, ENABLE);   //使能DMA中断
    // ADC1 regular channel 12 configuration
    ADC_InitStructure.ADC_Resolution           = ADC_Resolution_12b;            // 12bit
    ADC_InitStructure.ADC_ScanConvMode         = ENABLE;                        // 扫描模式
    ADC_InitStructure.ADC_ContinuousConvMode   = ENABLE;                        // 禁止连续转换，转换完毕进入中断
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None; // 禁止外部触发
    ADC_InitStructure.ADC_DataAlign            = ADC_DataAlign_Right;           // ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfConversion      = 3;                             // 4个通道
    ADC_Init(ADC3, &ADC_InitStructure);

    ADC_RegularChannelConfig(ADC3, ADC_Channel_10, 1, ADC_SampleTime_15Cycles); // M0_temp
    ADC_RegularChannelConfig(ADC3, ADC_Channel_12, 2, ADC_SampleTime_15Cycles); // M1_temp
    ADC_RegularChannelConfig(ADC3, ADC_Channel_11, 3, ADC_SampleTime_15Cycles); // Vbus

    DMA_Cmd(DMA2_Stream1, ENABLE);
    ADC_DMARequestAfterLastTransferCmd(ADC3, ENABLE); // Enable DMA request after last transfer (Single-ADC mode)
    ADC_DMACmd(ADC3, ENABLE);                         // ADC使能DMA
    ADC_Cmd(ADC3, ENABLE);
    ADC_SoftwareStartConv(ADC3); // 软件触发，ADC1开始在后台转换，永不停止
}

// void ADC1GetValue(void)
//{
//	DMA_Cmd(DMA2_Stream1,DISABLE);
//	DMA_SetCurrDataCounter(DMA2_Stream1,12);		//写计数器的值,需要失能DMA
//	DMA_Cmd(DMA2_Stream1,ENABLE);
//	ADC_SoftwareStartConv(ADC3);			//ADC软件触发使能
//	while(DMA_GetFlagStatus(DMA2_Stream1,DMA_FLAG_TCIF1) == RESET);
//	DMA_ClearFlag(DMA2_Stream1,DMA_FLAG_TCIF1);
//
// }
/****************************************************************************/
// DMA转换完毕，重新再开始，防止ADC错位
// void DMA2_Stream1_IRQHandler(void)
//{
//     if(DMA_GetITStatus(DMA2_Stream1, DMA_IT_TCIF1) == SET)
//     {
//				DMA_Cmd(DMA2_Stream1,DISABLE);
//			DMA_ClearITPendingBit(DMA2_Stream1,DMA_FLAG_TCIF1);
//         DMA_ClearFlag(DMA2_Stream1,DMA_FLAG_TCIF1 | DMA_FLAG_FEIF1 | DMA_FLAG_DMEIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_HTIF1);
////				while(DMA_GetCmdStatus(DMA2_Stream1) != DISABLE){}      //确保DMA可以被设置
////				DMA_SetCurrDataCounter(DMA2_Stream1,12);//数据传输量
//				DMA_Cmd(DMA2_Stream1, ENABLE);
////        ADC_SoftwareStartConv(ADC3);
//    }
//}
/****************************************************************************/
void ADC2_TRGO_Init(void)
{

    ADC_InitTypeDef ADC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE);

    ADC_InitStructure.ADC_Resolution           = ADC_Resolution_12b; // 12bit
    ADC_InitStructure.ADC_ScanConvMode         = ENABLE;             // 模式
    ADC_InitStructure.ADC_ContinuousConvMode   = DISABLE;            // 单次转换
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
    ADC_InitStructure.ADC_ExternalTrigConv     = ADC_ExternalTrigInjecConv_T1_TRGO; // 外部触发 T8_TRGO
    ADC_InitStructure.ADC_DataAlign            = ADC_DataAlign_Right;               // ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfConversion      = 3;                                 // 规则组只有1个通道
    ADC_Init(ADC2, &ADC_InitStructure);
    ADC_DMARequestAfterLastTransferCmd(ADC2, DISABLE);

    ADC_InjectedSequencerLengthConfig(ADC2, 3);                                 // 注入组只有一个通道
    ADC_InjectedChannelConfig(ADC2, ADC_Channel_1, 1, ADC_SampleTime_15Cycles); // 检测HALL_D
    ADC_InjectedChannelConfig(ADC2, ADC_Channel_2, 2, ADC_SampleTime_15Cycles); // 检测W
    ADC_InjectedChannelConfig(ADC2, ADC_Channel_3, 3, ADC_SampleTime_15Cycles); // 检测U
    //		ADC_InjectedChannelConfig(ADC2, ADC_Channel_15, 4, ADC_SampleTime_3Cycles);  //channel_10,检测m1_phw

    ADC_SetInjectedOffset(ADC2, ADC_InjectedChannel_1, 0);
    ADC_SetInjectedOffset(ADC2, ADC_InjectedChannel_2, 0);
    ADC_SetInjectedOffset(ADC2, ADC_InjectedChannel_3, 0);
    //		ADC_SetInjectedOffset(ADC2, ADC_InjectedChannel_4, 0);
    ADC_ExternalTrigInjectedConvEdgeConfig(ADC2, ADC_ExternalTrigInjecConvEdge_Rising);
    ADC_ExternalTrigInjectedConvConfig(ADC2, ADC_ExternalTrigInjecConv_T1_TRGO); // 外部触发 T1_TRGO
    ADC_AutoInjectedConvCmd(ADC2, DISABLE);
    ADC_InjectedDiscModeCmd(ADC2, DISABLE);

    ADC_ITConfig(ADC2, ADC_IT_JEOC, ENABLE); // 打开注入组完成中断
    ADC_Cmd(ADC2, ENABLE);
}
/****************************************************************************/

/****************************************************************************/
void ADC1_TRGO_Init(void)
{

    ADC_InitTypeDef ADC_InitStructure;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE);

    ADC_InitStructure.ADC_Resolution           = ADC_Resolution_12b; // 12bit
    ADC_InitStructure.ADC_ScanConvMode         = ENABLE;             // 模式
    ADC_InitStructure.ADC_ContinuousConvMode   = DISABLE;            // 单次转换
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_Rising;
    ADC_InitStructure.ADC_ExternalTrigConv     = ADC_ExternalTrigInjecConv_T1_TRGO; // 外部触发 T8_TRGO
    ADC_InitStructure.ADC_DataAlign            = ADC_DataAlign_Right;               // ADC数据右对齐
    ADC_InitStructure.ADC_NbrOfConversion      = 3;                                 // 规则组只有1个通道
    ADC_Init(ADC1, &ADC_InitStructure);
    ADC_DMARequestAfterLastTransferCmd(ADC1, DISABLE);

    ADC_InjectedSequencerLengthConfig(ADC1, 3);                                 // 注入组只有一个通道
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_15Cycles); // 检测HALL_A
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_5, 2, ADC_SampleTime_15Cycles); // 检测HALL_B
    ADC_InjectedChannelConfig(ADC1, ADC_Channel_6, 3, ADC_SampleTime_15Cycles); // 检测HALL_C
    //    ADC_InjectedChannelConfig(ADC1, ADC_Channel_14, 4, ADC_SampleTime_3Cycles);  //channel_10,检测m0_phC

    ADC_SetInjectedOffset(ADC1, ADC_InjectedChannel_1, 0);
    ADC_SetInjectedOffset(ADC1, ADC_InjectedChannel_2, 0);
    ADC_SetInjectedOffset(ADC1, ADC_InjectedChannel_3, 0);
    //		ADC_SetInjectedOffset(ADC1, ADC_InjectedChannel_4, 0);
    ADC_ExternalTrigInjectedConvEdgeConfig(ADC1, ADC_ExternalTrigInjecConvEdge_Rising);
    ADC_ExternalTrigInjectedConvConfig(ADC1, ADC_ExternalTrigInjecConv_T1_TRGO); // 外部触发 T1_TRGO
    ADC_AutoInjectedConvCmd(ADC1, DISABLE);
    ADC_InjectedDiscModeCmd(ADC1, DISABLE);

    ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE); // 打开注入组完成中断
    ADC_Cmd(ADC1, ENABLE);
}
/*****************************************************************************/

uint8_t tamaCnt  = 1;
uint8_t tamaFlag = 0;
extern MOTORController M0, M1;
uint32_t count         = 0;
float testUd           = 0.0f;
float testUq           = 0.2f;
int16_t ANGLE          = 0;
int16_t Angle_test     = 0;
float testId           = 0.1f;
float testPI_P         = 0.5f;
float testPI_I         = 10.0f;
int16_t AngleAdd       = 20;
int16_t AngleAddtst    = -20;
int16_t angle_compens  = 0;
float TEST_X           = 0;
float TEST_Y           = 0;
int16_t difftest       = 0;
float IDrag            = 5.0f;
uint8_t sensorlessFlag = 0;
uint8_t testflag       = 0;
uint8_t testflag2      = 0;
// tim1配置为中央对齐模式，一个周期update两次，每(2+1)次触发一次，所以进入中断的频率为16KHz，
void ADC_IRQHandler(void) // ADC中断
{
    GPIO_ToggleBits(TESTGPIO_Port, TESTGPIO_Pin);
    //	GPIO_ResetBits(LED_GPIO_Port,LED_Pin);
    if ((ADC1->SR & ADC_SR_JEOC) == ADC_SR_JEOC) {
        //				GPIO_ToggleBits(LED_GPIO_Port,LED_Pin);
        ADC1->SR = ~ADC_SR_JEOC;

        //        ENCOD_PLL.ADC_X = ADC1->JDR3;
        ENCOD_PLL.ADC_Z = ADC1->JDR2 - 2055;
        ENCOD_PLL.ADC_W = ADC1->JDR3 - 2038;
        ENCOD_PLL.ADC_W = ENCOD_PLL.ADC_W * 1.0303f;

        ENCOD_PLL.ADC_X = ADC1->JDR1;
        ENCOD_PLL.ADC_X = ENCOD_PLL.ADC_X - 2052; // 451
    }
    if ((ADC2->SR & ADC_SR_JEOC) == ADC_SR_JEOC) {
        ADC2->SR = ~ADC_SR_JEOC;

        ENCOD_PLL.ADC_Y = ADC2->JDR1;
        ENCOD_PLL.ADC_Y = ENCOD_PLL.ADC_Y - 2042; // 458
        ENCOD_PLL.ADC_Y = ENCOD_PLL.ADC_Y * 0.9676238f;
        M0.phA          = ADC2->JDR3;
        M0.phC          = ADC2->JDR2;
    }

    ADC_ClearFlag(ADC1, ADC_FLAG_JEOC | ADC_FLAG_EOC | ADC_FLAG_AWD | ADC_FLAG_JSTRT | ADC_FLAG_STRT);
    ADC_ClearFlag(ADC2, ADC_FLAG_JEOC | ADC_FLAG_EOC | ADC_FLAG_AWD | ADC_FLAG_JSTRT | ADC_FLAG_STRT);

    ADC_ITConfig(ADC1, ADC_IT_JEOC, ENABLE);
    ADC_ITConfig(ADC2, ADC_IT_JEOC, ENABLE);
    //		M0.gain_a = testAmp;
    //		M0.gain_b = testAmp;
    //		M0.gain_c = testAmp;
    PhaseCurrent(&M0, &PhasecurrentM0);
    //		PhaseCurrent(&M1,&PhasecurrentM1);

    /**电流保护**/

    /**电流保护**/
    /*****有感****/
    Orth_CAL(&ENCOD_PLL);
    Pgy_Fast_Loop();
    if (count < 5000) {
        count++;
        //		ENCOD_PLL.Angle_E_sum = 0;
        //		ENCOD_PLL.vari_Eangle = 0;
        ENCOD_PLL.EangleLast = ENCOD_PLL.ANGLE_ROTOR_EINT_Calib;
    } else {
        EAngleSum();
        ACCurrentProt(&M0, &PhasecurrentM0);
    }

    PLLSpeedCal();
    /*****有感****/

    /*****无感****/
    //		ENCOD_PLL.SPEED_ROTOR_FILT = ENCOD_PLL.SPEED_ROTOR_S;
    if (testflag == 1) {
        Idtest();
    }
    //		Pos_Esti_Calc();
    //		ENCOD_PLL.ANGLE_ROTOR_MINT_Calib = SMO_Esti.SmoPLL.PLL_AngleOut_MINTCalib;
    /*****无感****/
    /*******电机运行状态切换*******/
    switch (motor_status) {
        case Standby: {
            MosShut();
            FSM_SoftShut();
            break;
        }
        case ADCOfst: {
            Current_calibrateOffsets(&M0, &M1);
            break;
        }
        case PrePos: {
            VolMotorPrepos(&M0, &M1);

            break;
        }
        case Running: {
            TimGpioReInit();
            /**FOC主程序**/

            //            ACCurrentProt(&M0, &PhasecurrentM0);
            //            GPIO_ResetBits(GPIOE, GPIO_Pin_5);
            //            			ANGLE += AngleAdd;
            //            			FaultCurrentRef(1,ANGLE);
            //			FaultCurrentRef2F(1,ANGLE);
            //			if(FaultSenseEnab)
            //			{
            //				FaultSense();
            //			}
            //              			M0.PID_d.P = testPI_P;
            //            				M1.PID_d.P = testPI_P;

            //            				M0.PID_d.I = testPI_I;
            //            				M1.PID_d.I = testPI_I;
            //
            //              			M0.PID_q.P = testPI_P;
            //            				M1.PID_q.P = testPI_P;

            //            				M0.PID_q.I = testPI_I;
            //            				M1.PID_q.I = testPI_I;
            //
            /********无感测试*********/
            //            if (!sensorlessFlag) {
            //                M1.enable = 1;
            //                M0.enable = 1;
            //                M0.electrical_angle_int += AngleAdd;

            //                //					M0.electrical_angle_int = ABZEncoder.ANGLE_ROTOR_INT ;//+ ABZEncoder.InitPos;
            //                //					M1.electrical_angle_int = M0.electrical_angle_int + 32768 ;

            //                M0.PID_q.ref = IDrag;

            //                M0.PID_d.ref = 0.0f;

            //

            //                loopFOC(&M0);
            //
            //            } else {

            //                M0.enable = 1;

            //                					M0.PID_d.ref = 0;
            //                //					M1.PID_d.ref = 0;
            //                //					M0.PID_Z.ref = 0;
            //                //					M1.PID_Z.ref = 0;
            //                //					voltage_power_supply = 48.0f;
            //                M0.electrical_angle_int = SMO_Esti.SmoPLL.PLL_AngleOut; // + ABZEncoder.InitPos;

            //                loopFOC(&M0);

            //
            //            }
            /********无感测试*********/

            /******有感测试******/
            M0.enable = 1;

            M0.PID_d.ref = 0;
            //					M1.PID_d.ref = 0;
            //					M0.PID_Z.ref = 0;
            //					M1.PID_Z.ref = 0;
            //					voltage_power_supply = 48.0f;
            M0.electrical_angle_int = ENCOD_PLL.ANGLE_ROTOR_INT; // + ABZEncoder.InitPos;

            loopFOC(&M0);
            /******有感测试******/

            //						M0.enable = 1;
            //						M0.PID_d.ref = 0;

            //							M0.electrical_angle_int = ENCOD_PLL.ANGLE_ROTOR_INT + angle_compens;// + ABZEncoder.InitPos;

            //							loopFOC(&M0);

            /**********电压开环测试*******/

            //				ANGLE += AngleAddtst;
            //				testUd = 0.0f;

            //				setPhaseVoltage(&M0,testUq,testUd,ANGLE);

            /**********电压开环测试*******/

            /*****六相电流环PI调试*****/
            //				M0.enable = 1;
            ////				voltage_power_supply = 48.0f;
            //				M0.electrical_angle_int = 0;
            //				M0.PID_d.ref = testId;

            //
            //				M0.PID_d.P = testPI_P;

            //				M0.PID_d.I = testPI_I;

            //				M0.PID_q.P = 0;

            //
            //				M0.PID_q.I = 0;

            //				loopFOC(&M0);
            /*****电流环PI调试*****/

            //		/**********电流环预定位*******/
            //				M0.enable = 1;

            ////				voltage_power_supply = 48.0f;
            //				M0.electrical_angle_int = 0;
            ////				M0.electrical_angle_int = ENCODER.ANGLE_ROTOR_INT;

            //				M0.PID_q.ref = CurrentLoopAngleAddInit;

            //				M0.PID_d.ref = 0.0f;
            //				loopFOC(&M0);
            /**********电流环预定位*******/

            //		/**********电流环强拉测试*******/
            //				M0.enable = 1;

            //				M0.electrical_angle_int += AngleAdd;

            //				M0.PID_q.ref = IDrag;

            //				M0.PID_d.ref = 0.0f;
            //				loopFOC(&M0);
            /**********电流环强拉测试*******/

            break;
        }
        case ERROR: {
            MosShut();
            break;
        }

        case AtoCalib: {
            OrthAUTOCalib();
            break;
        }

        case ZeroCurCalib: {
            //				M1.enable = 1;
            M0.enable    = 1;
            M0.PID_d.ref = 0;
            //				M1.PID_d.ref = 0;
            M0.PID_q.ref = 0;
            //				M1.PID_q.ref = 0;
            //					voltage_power_supply = 48.0f;
            M0.electrical_angle_int = ENCOD_PLL.ANGLE_ROTOR_INT + angle_compens; // + ABZEncoder.InitPos;
            M1.electrical_angle_int = M0.electrical_angle_int + 32768;

            loopFOC(&M0);
            //					loopFOC(&M1);

            EMFAngleTst.ADC_X = -AlphBetaVolM0.alph * voltage_power_supply / _SQRT3;
            EMFAngleTst.ADC_Y = AlphBetaVolM0.beta * voltage_power_supply / _SQRT3;
            Orth_CAL_f(&EMFAngleTst);
            //		difftest = EMFAngleTst.ANGLE_ROTOR_MINT_Calib - ENCOD_PLL.ANGLE_ROTOR_MINT_Calib;
            //			if(abs(difftest)>32768)
            //			{
            //				if(difftest>0)
            //				{
            //					difftest = -(65536%difftest);
            //				}
            //				else
            //				{
            //					difftest = (65536%difftest);
            //				}
            //			}
            break;
        }

        case AxisHallCalib: {
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
            break;
        }

        case WriteToFlash: {
            //-----------Write to Flash Begin------------
            WriteToFlash_FLAG = 1;
            //-----------Write to Flash End--------------
            motor_status = Standby;
            break;
        }
    }

    motor_status_last = motor_status;
    //		GPIO_SetBits(LED_GPIO_Port,LED_Pin);
    /*******电机运行状态切换*******/
}
/****************************************************************************/
/***母线电压测量***/
void BusVolSense(void)
{
    uint16_t BusVoltageRaw_sum = 0, BusVoltageRaw = 0;

    for (uint8_t num = 0; num < 4; num++) {
        BusVoltageRaw_sum += adc0_value[2 + num * 3];
    }
    BusVoltageRaw        = BusVoltageRaw_sum >> 2;
    voltage_power_supply = 0.02739926739926739 * BusVoltageRaw;
}

/***板子温度测量***/
void BoardTemSense(void)
{
    uint16_t TemRaw_Sum = 0, TemRaw = 0, middle;

    for (uint8_t num = 0; num < 4; num++) {
        TemRaw_Sum += adc0_value[0 + num * 3];
    }
    TemRaw = TemRaw_Sum >> 2;

    uint16_t high = 159, low = 0;

    while (1) {
        middle = (high + low) >> 1;
        if (TemRaw > hTEMPERATURE_TABLE[middle]) {
            high = (high + low) >> 1;
        } else if (TemRaw < hTEMPERATURE_TABLE[middle]) {
            low = (high + low) >> 1;
        } else {
            BoardTemp = middle - 40;
            break;
        }
        if (high == low + 1) {
            BoardTemp = middle - 40;
            break;
        }
    }
}
/***定子温度测量***/
void StatorTemSense(void)
{
    uint16_t STemRaw_Sum = 0, STemRaw = 0, Smiddle;

    for (uint8_t num = 0; num < 4; num++) {
        STemRaw_Sum += adc0_value[1 + num * 3];
    }
    STemRaw = STemRaw_Sum >> 2;

    uint16_t Shigh = 240, Slow = 0;

    while (1) {
        if (STemRaw > 4087) {
            StatorTemp = 0; // 开路
            break;
        }
        if (STemRaw < 368) {
            StatorTemp = 1; // 短路
            break;
        }

        Smiddle = (Shigh + Slow) >> 1;
        if (STemRaw > hTEMPERATURE_TABLE[Smiddle]) {
            Shigh = (Shigh + Slow) >> 1;
        } else if (STemRaw < hTEMPERATURE_TABLE[Smiddle]) {
            Slow = (Shigh + Slow) >> 1;
        } else {
            StatorTemp = Smiddle - 40;
            break;
        }
        if (Shigh == Slow + 1) {
            StatorTemp = Smiddle - 40;
            break;
        }
    }
}

void ILoopDrag_D(int16_t AngleAdd, float DragCur)
{
    M1.enable = 1;
    M0.enable = 1;

    //				voltage_power_supply = 48.0f;
    M0.electrical_angle_int += AngleAdd;
    M1.electrical_angle_int = M0.electrical_angle_int + 32768;
    //				Diff = M0.electrical_angle_int - ABZEncoder.ANGLE_ROTOR_INT;
    //					M0.electrical_angle_int = ABZEncoder.ANGLE_ROTOR_INT ;//+ ABZEncoder.InitPos;
    //					M1.electrical_angle_int = M0.electrical_angle_int + 32768 ;

    M0.PID_q.ref = 0.0f;
    M1.PID_q.ref = 0.0f;

    M0.PID_d.ref = DragCur;
    M1.PID_d.ref = DragCur;
    loopFOC(&M0);
    loopFOC(&M1);
}

void ILoopPos(int16_t pos, float DragCur)
{
    M1.enable = 1;
    M0.enable = 1;

    //				voltage_power_supply = 48.0f;
    M0.electrical_angle_int = pos;
    M1.electrical_angle_int = M0.electrical_angle_int + 32768;
    //				Diff = M0.electrical_angle_int - ABZEncoder.ANGLE_ROTOR_INT;
    //					M0.electrical_angle_int = ABZEncoder.ANGLE_ROTOR_INT ;//+ ABZEncoder.InitPos;
    //					M1.electrical_angle_int = M0.electrical_angle_int + 32768 ;

    M0.PID_q.ref = 0;
    M1.PID_q.ref = 0;

    M0.PID_d.ref = DragCur;
    M1.PID_d.ref = DragCur;
    loopFOC(&M0);
    loopFOC(&M1);
}

float SineWave    = 0;
float SineWaveInt = 0;
int16_t Angle     = 0;
int16_t angleinc  = 65;
void Idtest(void)
{
    //	static float IdH = 2;
    //	static float IdL = -2;
    //	static uint8_t ChangeFlag = 0;
    //	static int16_t AngleInc = 0;
    //
    //	if(ChangeFlag == 0)
    //	{
    //		testId = IdH;
    //		ChangeFlag = 1;
    //	}
    //	else
    //	{
    //		testId = IdL;
    //		ChangeFlag = 0;
    //	}

    Trig_Components Local_Vector_ComponentsTEST;
    Local_Vector_ComponentsTEST = MCM_Trig_Functions(Angle);

    SineWave    = Local_Vector_ComponentsTEST.hSin;
    testId      = SineWave;
    SineWaveInt = 1.5 * (SineWave + 1);
    //		ENCOD_PLL.ADC_Y = Local_Vector_ComponentsTEST.hCos;

    Angle = Angle + angleinc;
}

/****************************************************************************/
