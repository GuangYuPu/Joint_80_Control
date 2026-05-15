#include "FlashRW.h"
#include "string.h"
//#include "ABZ.h"
#include "adc.h"
uint16_t write_data[DATA_FLASH_SAVE_NUM];
uint8_t read_data[DATA_FLASH_SAVE_NUM];

//通过地址获取扇区位置
uint16_t STMFLASH_GetFlashSector(u32 addr)
{
	if(addr<ADDR_FLASH_SECTOR_1)return FLASH_Sector_0;
	else if(addr<ADDR_FLASH_SECTOR_2)return FLASH_Sector_1;
	else if(addr<ADDR_FLASH_SECTOR_3)return FLASH_Sector_2;
	else if(addr<ADDR_FLASH_SECTOR_4)return FLASH_Sector_3;
	else if(addr<ADDR_FLASH_SECTOR_5)return FLASH_Sector_4;
	else if(addr<ADDR_FLASH_SECTOR_6)return FLASH_Sector_5;
	else if(addr<ADDR_FLASH_SECTOR_7)return FLASH_Sector_6;
	else if(addr<ADDR_FLASH_SECTOR_8)return FLASH_Sector_7;
	else if(addr<ADDR_FLASH_SECTOR_9)return FLASH_Sector_8;
	else if(addr<ADDR_FLASH_SECTOR_10)return FLASH_Sector_9;
	else if(addr<ADDR_FLASH_SECTOR_11)return FLASH_Sector_10; 
	return FLASH_Sector_11;	
}

//将数据写入内存 16位数据
int write_flash(uint16_t *FlashWriteBuf)
{
	
	uint32_t StartAddr;
	StartAddr = FLASH_SAVE_ADDR;
	StartAddr = StartAddr;
	FLASH_Unlock();	//解锁
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | 
                  FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR|FLASH_FLAG_PGSERR);

	if (FLASH_COMPLETE != FLASH_EraseSector(STMFLASH_GetFlashSector(StartAddr),VoltageRange_3)) //擦除扇区内容
    {		
		return TEST_ERROR;
	}
	
	for (int i = 0; i < DATA_FLASH_SAVE_NUM; i++)
	{
		if (FLASH_COMPLETE != FLASH_ProgramByte(StartAddr, FlashWriteBuf[i]))	//写入16位数据
		{	
			return TEST_ERROR;
		}
		StartAddr += 1;	//16位数据偏移两个位置
	}

	FLASH_Lock();	//上锁

	return TEST_SUCCESS;
}

//从内存读数据 16位数据
void read_flash(uint8_t *FlashReadBuf)
{
	uint32_t StartAddr = FLASH_SAVE_ADDR;
	for (int i = 0; i < DATA_FLASH_SAVE_NUM; i++)
	{
		FlashReadBuf[i] = *(__IO uint8_t*)StartAddr;
		StartAddr += 1;
	}
}

void write_to_flash_all(OrthATOCALIB_T *V)
{
	uint8_t temp1,temp2;
	write_data[0] = V->EroMeanRec >> 8;
	write_data[1] = V->EroMeanRec % 256;
	if(V->DirDetect == 1)
	{
		write_data[2] = 1;
	}
	else if(V->DirDetect == -1)
	{
		write_data[2] = 0;
	}
	
	write_data[3] = V->X_Ofst >> 8;
	write_data[4] = V->X_Ofst % 256;
	
	write_data[5] = V->X_Amp >> 8;
	write_data[6] = V->X_Amp % 256;
	
	write_data[7] = V->Y_Ofst >> 8;
	write_data[8] = V->Y_Ofst % 256;
	
	write_data[9] = V->Y_Amp >> 8;
	write_data[10] = V->Y_Amp % 256;
	/*
		//这里就可以写入一些参数 如kp、ki
		write_data[0] = kp;
		write_data[1] = ki;
	*/
	if(TEST_SUCCESS!=write_flash(write_data))	
		return;  //写入错误

}

void write_to_flash(uint16_t PosCalib,int8_t dirDect)
{
	uint8_t temp1,temp2;
	write_data[0] = PosCalib >> 8;
	write_data[1] = PosCalib % 256;
	if(dirDect == 1)
	{
		write_data[2] = 1;
	}
	else if(dirDect == -1)
	{
		write_data[2] = 0;
	}
	/*
		//这里就可以写入一些参数 如kp、ki
		write_data[0] = kp;
		write_data[1] = ki;
	*/
	if(TEST_SUCCESS!=write_flash(write_data))	
		return;  //写入错误

}


void read_from_flash(OrthATOCALIB_T *M)
{

	memset(read_data, 0, sizeof(read_data));
	read_flash(read_data);
	M->EroMeanRec = read_data[0]<<8|read_data[1];
	if(read_data[2] == 0)
	{
		M->DirDetect = -1;
	}
	else if (read_data[2] == 1)
	{
		M->DirDetect = 1;
	}
	
	M->X_Ofst = read_data[3]<<8|read_data[4];
	M->X_Amp = read_data[5]<<8|read_data[6];
	M->X_Amp_f = M->X_Amp*1e-4;;
	
	
	M->Y_Ofst = read_data[7]<<8|read_data[8];
	M->Y_Amp = read_data[9]<<8|read_data[10];
	M->Y_Amp_f = M->Y_Amp*1e-4;;

}