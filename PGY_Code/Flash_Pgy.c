#include "Flash_Pgy.h"
#include "stm32f4xx_flash.h"

/**
  * @brief  将 int16_t 数组写入 Flash
  * @param  DATA_ADDR: 待写入数据的数组指针
  * @param  Data_Length: 数组长度（单位：int16_t）
  * @param  ADDR_FLASH_SECTOR: Flash 扇区起始地址
  * @retval None
  */

uint8_t WriteToFlash_FLAG = 0;

void WriteToFlash_PGY(int16_t *DATA_ADDR, uint16_t Data_Length, u32 ADDR_FLASH_SECTOR)
{
    uint16_t i;
    uint32_t address = ADDR_FLASH_SECTOR;
    FLASH_Status status;

    // 解锁 Flash
    FLASH_Unlock();

    // 清除所有 Flash 标志位
    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR |
                    FLASH_FLAG_PGAERR | FLASH_FLAG_PGPERR | FLASH_FLAG_PGSERR);

    // 根据地址确定扇区编号（需根据实际地址映射调整）
    uint32_t sector = 0;
    if (address >= PGY_ADDR_FLASH_SECTOR_0 && address < PGY_ADDR_FLASH_SECTOR_1) {
        sector = FLASH_Sector_0;
    } else if (address >= PGY_ADDR_FLASH_SECTOR_1 && address < PGY_ADDR_FLASH_SECTOR_2) {
        sector = FLASH_Sector_1;
    } else if (address >= PGY_ADDR_FLASH_SECTOR_2 && address < PGY_ADDR_FLASH_SECTOR_3) {
        sector = FLASH_Sector_2;
    } else if (address >= PGY_ADDR_FLASH_SECTOR_3 && address < PGY_ADDR_FLASH_SECTOR_4) {
        sector = FLASH_Sector_3;
    } else if (address >= PGY_ADDR_FLASH_SECTOR_4 && address < PGY_ADDR_FLASH_SECTOR_5) {
        sector = FLASH_Sector_4;
    } else if (address >= PGY_ADDR_FLASH_SECTOR_5 && address < PGY_ADDR_FLASH_SECTOR_6) {
        sector = FLASH_Sector_5;
    } else if (address >= PGY_ADDR_FLASH_SECTOR_6 && address < PGY_ADDR_FLASH_SECTOR_7) {
        sector = FLASH_Sector_6;
    } else if (address >= PGY_ADDR_FLASH_SECTOR_7 && address < PGY_ADDR_FLASH_SECTOR_8) {
        sector = FLASH_Sector_7;
    } else if (address >= PGY_ADDR_FLASH_SECTOR_8 && address < PGY_ADDR_FLASH_SECTOR_9) {
        sector = FLASH_Sector_8;
    } else if (address >= PGY_ADDR_FLASH_SECTOR_9 && address < PGY_ADDR_FLASH_SECTOR_10) {
        sector = FLASH_Sector_9;
    } else if (address >= PGY_ADDR_FLASH_SECTOR_10) {
        sector = FLASH_Sector_10;
    }

    // 擦除指定扇区
    status = FLASH_EraseSector(sector, VoltageRange_3);
    if (status != FLASH_COMPLETE) {
        // 擦除失败处理（可根据需要添加错误处理）
        FLASH_Lock();
        return;
    }

    // 写入数据（按半字写入，int16_t 占 2 字节）
    for (i = 0; i < Data_Length; i++) {
        status = FLASH_ProgramHalfWord(address, (uint16_t)(*(DATA_ADDR + i)));
        if (status != FLASH_COMPLETE) {
            // 写入失败处理
            // FLASH_Lock();
            // return;
        }
        address += 2;  // 地址递增 2 字节
    }

    // 锁定 Flash
    FLASH_Lock();
}

/**
  * @brief  从 Flash 读取 int16_t 数组
  * @param  DATA_ADDR: 用于存储读取数据的数组指针
  * @param  Data_Length: 数组长度（单位：int16_t）
  * @param  ADDR_FLASH_SECTOR: Flash 扇区起始地址
  * @retval None
  */
void ReadFromFlash_PGY(int16_t *DATA_ADDR, uint16_t Data_Length, u32 ADDR_FLASH_SECTOR)
{
    uint16_t i;
    uint32_t address = ADDR_FLASH_SECTOR;

    for (i = 0; i < Data_Length; i++) {
        *(DATA_ADDR + i) = *(__IO int16_t*)address;
        address += 2;  // 地址递增 2 字节
    }
}