
#include "usart2.h"

#include "tamagawa.h"
#include "string.h"
#include "delay.h"
#include "foc_utils.h"
//extern UART_HandleTypeDef huart3;
struct TamagawaInterface ti;
uint8_t tx_size = 0, rx_size = 0;
uint8_t tx;

uint8_t USART3_RX_Buff[11];
uint8_t tamagawa_rx_cnt = 0;
uint8_t usart_data[5];
uint8_t TXData[1];

float position  = 0;
short turns = 0;

void tx_prepare(uint8_t *tx, uint8_t *tx_size, uint8_t *rx_size);
void tamagawa_tx(uint8_t tx_size, uint8_t *tx_buffer);
uint8_t crc(uint8_t *s, uint8_t len);





void tamagawa_read(uint8_t data_id)
{
		TXData[0] = 0x02;
    ti.data_id = data_id;
    tamagawa_rx_cnt = 0;
    tx_prepare(&tx, &tx_size, &rx_size);
    tamagawa_tx(tx_size, &tx);
//	while (tamagawa_rx_cnt<rx_size)
//	{
//	}
}

void tx_prepare(uint8_t *tx, uint8_t *tx_size, uint8_t *rx_size)
{
 switch(ti.data_id)
    {
        case DATA_ID_0:
            *tx = 0x02;
            *tx_size = 1;
//            *rx_size = 6;
				//位置编码器协议与tamagawa不同，收到的数据只有5个
						*rx_size = 5;
            break;

        case DATA_ID_1:
            *tx = 0x8A;
            *tx_size = 1;
            *rx_size = 6;
            break;

        case DATA_ID_2:
            *tx = 0x92;
            *tx_size = 1;
            *rx_size = 4;
            break;

        case DATA_ID_3:
            *tx = 0x1A;
            *tx_size = 1;
            *rx_size = 11;
            break;

        case DATA_ID_7:
            *tx = 0xBA;
            *tx_size = 1;
            *rx_size = 6;
            break;

        case DATA_ID_8:
            *tx = 0xC2;
            *tx_size = 1;
            *rx_size = 6;
            break;

        case DATA_ID_C:
            *tx = 0x62;
            *tx_size = 1;
            *rx_size = 6;
            break;

        case DATA_ID_6:
            *tx++ = 0x32;
            *tx++ = ti.tx.adf;
            *tx++ = ti.tx.edf;
            *tx_size = 4;
            *rx_size = 4;
            *tx = crc(tx - 3, *tx_size - 1);
            break;

        case DATA_ID_D:
            *tx++ = 0xEA;
            *tx++ = ti.tx.adf;
            *tx_size = 3;
            *rx_size = 4;
            *tx = crc(tx - 2, *tx_size - 1);
            break;

        default:
            break;
    }
}

void tamagawa_tx(uint8_t tx_size, uint8_t *tx_buffer)
{
//    while(tx_size--)
//    {
        GPIO_SetBits(RS485_EN_GPIO_Port, RS485_EN_Pin);
				uint8_t tx_temp;
				tx_temp = *tx_buffer++;
//        snd2_buff[tx_temp] = *tx_buffer++;
//        USART_SendData(USART3,tx_temp);
			USART3->DR =(uint8_t)(0x02 & 0xFFU);
//			while(USART_GetFlagStatus(USART3,USART_FLAG_TC)==RESET){}
		while((((USART3->SR&USART_FLAG_TC) == USART_FLAG_TC) ? SET : RESET) == RESET)
		{
//			if(((TIM1->SR&TIM_FLAG_Update) == TIM_FLAG_Update ? SET : RESET) == SET)
//			{
//				break;
//			}
//			if((ADC1->SR & ADC_SR_JEOC) == ADC_SR_JEOC)
//			{
//				break;
//			}
		};
//			delay_us(86);
        GPIO_ResetBits(RS485_EN_GPIO_Port, RS485_EN_Pin);
//    }
}

void rx_parse(uint8_t *p)
{
    switch(ti.data_id)
    {
    case DATA_ID_0:
    {
            ti.rx.cf = p[0];
            ti.rx.sf = p[1];
//            ti.rx.abs = p[2] | (p[3] << 8) | (p[4] << 16);
//            ti.rx.crc = p[5];
//            position  = ((double)(ti.rx.abs) / 131071) * 360;
						ti.rx.abs = p[2] | (p[3] << 8) ;
						ti.rx.crc = p[4];
						position  = ((double)(ti.rx.abs) / 16384) * 360;
						if(position>180)
						{
							position -= 360;
						}
        break;
    }
    case DATA_ID_7:
        break;
    case DATA_ID_8:
        break;
    case DATA_ID_C:
        ti.rx.cf = p[0];
        ti.rx.sf = p[1];
        ti.rx.abs = p[2] | (p[3] << 8) | (p[4] << 16);
        ti.rx.crc = p[5];
        break;

    case DATA_ID_1:
        ti.rx.cf = p[0];
        ti.rx.sf = p[1];
        ti.rx.abm = p[2] | (p[3] << 8) | (p[4] << 16);
        ti.rx.crc = p[5];
        turns = ti.rx.abm;
        break;

    case DATA_ID_2:
        ti.rx.cf = p[0];
        ti.rx.sf = p[1];
        ti.rx.enid = p[2];
        ti.rx.crc = p[3];
        break;

    case DATA_ID_3:
        ti.rx.cf = p[0];
        ti.rx.sf = p[1];
        ti.rx.abs = p[2] | (p[3] << 8) | (p[4] << 16);
        ti.rx.enid = p[5];
        ti.rx.abm = p[6] | (p[7] << 8) | (p[8] << 16);
        ti.rx.almc = p[9];
        ti.rx.crc = p[10];
        position  = ((double)(ti.rx.abs) / 131071) * 360;
        turns = ti.rx.abm;

        break;

    case DATA_ID_6:
        break;
    case DATA_ID_D:
        ti.rx.cf = p[0];
        ti.rx.adf = p[1];
        ti.rx.edf = p[2];
        ti.rx.crc = p[3];
        break;

    default:
        break;
    }

}

uint8_t crc(uint8_t *s, uint8_t len)
{
    uint8_t crc = 0;
    uint8_t val;
    int  i, j;
    uint8_t data[12];

    strncpy((char *)data, (const char *)s, len);

    for(i = 0; i < len; i++)
        for(j = 0; j < 8; j++)
        {
            val = (data[i] >> 7) ^ (crc >> 7);

            crc <<= 1;
            data[i] <<= 1;
            crc |= val;
        }

    return crc;
}

uint8_t cnt = 0;
void USART3_IRQHandler(void)//USART2_IRQHandler 
{

//	GPIO_ToggleBits(LED_GPIO_Port,LED_Pin);
	if(USART_GetITStatus(USART3,USART_IT_IDLE) == SET)
    {
			USART_ClearFlag(USART3, USART_FLAG_IDLE);   
        rcv2_cntr=USART3->SR;
        rcv2_cntr=USART3->DR;    
        DMA_Cmd(DMA1_Stream1,DISABLE);                            
//		if(tamagawa_rx_cnt < rx_size){
////			USART3_RX_Buff[tamagawa_rx_cnt] = usart_data;
//			tamagawa_rx_cnt++;
//		}
			
		
		for(uint8_t numrx = 0; numrx<5;numrx++)
			{
				USART3_RX_Buff[numrx] = usart_data[numrx];
			}
			
			rx_parse(USART3_RX_Buff);

		DMA_ClearFlag(DMA1_Stream1,DMA_FLAG_TCIF1 | DMA_FLAG_FEIF1 | DMA_FLAG_DMEIF1 | DMA_FLAG_TEIF1 | DMA_FLAG_HTIF1);
     while(DMA_GetCmdStatus(DMA1_Stream1) != DISABLE){}      //确保DMA可以被设置
     DMA_SetCurrDataCounter(DMA1_Stream1,5);//数据传输量
     DMA_Cmd(DMA1_Stream1,ENABLE);//使能DMA                          //开启DMA传输
    }
}

void ZeroTama(void)
{

		static uint8_t ZTnum = 0;
		
    GPIO_SetBits(RS485_EN_GPIO_Port, RS485_EN_Pin);
		USART3->DR =(uint8_t)(0xC2 & 0xFFU);;
		while((((USART3->SR&USART_FLAG_TC) == USART_FLAG_TC) ? SET : RESET) == RESET)
		{
		};
    GPIO_ResetBits(RS485_EN_GPIO_Port, RS485_EN_Pin);
		
		ZTnum++;
		
		if(ZTnum == 20)
		{
			TamaZerosFlag = 0;
		}

}

//void DMA1_Stream1_IRQHandler(void)
//{
////        rcv2_cntr=USART3->SR;
////        rcv2_cntr=USART3->DR;    //清除中断标志USART_ClearITPendingBit(USART2,USART_IT_RXNE)
//        DMA_Cmd(DMA1_Stream1,DISABLE);                            //关闭DMA传输
//			GPIO_ToggleBits(LED_GPIO_Port,LED_Pin);
////	if(tamagawa_rx_cnt < rx_size){
////		USART3_RX_Buff[tamagawa_rx_cnt] = usart_data;
////		tamagawa_rx_cnt++;
////	}
//		
////		if(tamagawa_rx_cnt == rx_size)
////		{
////			rx_parse(USART3_RX_Buff);
////		}
//		
//     while(DMA_GetCmdStatus(DMA1_Stream1) != DISABLE){}      //确保DMA可以被设置
//     DMA_SetCurrDataCounter(DMA1_Stream1,5);//数据传输量
//     DMA_Cmd(DMA1_Stream1,ENABLE);//使能DMA                          //开启DMA传输
//}