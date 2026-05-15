
#include "MyProject.h"

/*****************************************************************************/
void SPI3_Init_(uint8_t x, uint16_t spi_cpol)
{

}
/*****************************************************************************/
uint16_t SPIx_ReadWriteByte(uint16_t byte)
{
    while(RESET == spi_i2s_flag_get(SPI2, SPI_FLAG_TBE))
        ;

    /* send byte through the SPI5 peripheral */
    spi_i2s_data_transmit(SPI2, byte);

    /* wait to receive a byte */
    while(RESET == spi_i2s_flag_get(SPI2, SPI_FLAG_RBNE))
        ;

    /* return the byte read from the SPI bus */
    return(spi_i2s_data_receive(SPI2));
}
/******************************************************************************/

