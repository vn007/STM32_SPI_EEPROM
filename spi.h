/*-------------------------------------------*/
/*  Header for SPI utilities                 */
/*-------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __SPI_H
#define __SPI_H

#include "stm32f10x_spi.h"

int spiReadWrite(SPI_TypeDef* SPIx, uint8_t *rbuf, const uint8_t *tbuf, int num);
uint8_t spiXByte(SPI_TypeDef* SPIx, uint8_t out);

extern SPI_TypeDef *AppSPI;
extern SPI_InitTypeDef AppSPIinitStruct;

extern GPIO_TypeDef *AppGPIO;
extern GPIO_InitTypeDef App_GPIO_initStruct;

/*********************************************/
/*********************************************/
void AppSPI_Init(void);

#endif // __SPI_H
