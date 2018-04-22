/*-------------------------------------------*/
/*  SPI utilities                            */
/*  утилиты SPI                              */
/*-------------------------------------------*/

#include "spi.h"

/*-------------------------------------------*/

#define NOP	0xFF

/*********************************************/
/*********************************************/
void AppSPI_Init(){

	// Clocking ports and SPI itsef
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	// SPI pins init
    App_GPIO_initStruct.GPIO_Pin = GPIO_Pin_5 | GPIO_Pin_7  ;		// pin5 - SCK, pin7 - MOSI
	App_GPIO_initStruct.GPIO_Mode = GPIO_Mode_AF_PP;
    App_GPIO_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(AppGPIO, &App_GPIO_initStruct);	

    App_GPIO_initStruct.GPIO_Pin = GPIO_Pin_6;						// pin6 MISO
	App_GPIO_initStruct.GPIO_Mode = GPIO_Mode_IPU;
    App_GPIO_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(AppGPIO, &App_GPIO_initStruct);	

	// NSS, W (write protect) and HOLD pins init
    App_GPIO_initStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_8 | GPIO_Pin_9 ;		// pin10 - CS, pin PA8 - W, pin PA9 - HOLD
    App_GPIO_initStruct.GPIO_Pin |= GPIO_Pin_0;									//LV - test pins for debugging
	App_GPIO_initStruct.GPIO_Mode = GPIO_Mode_Out_PP;
    App_GPIO_initStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(AppGPIO, &App_GPIO_initStruct);	


	// SPI init
	SPI_StructInit(&AppSPIinitStruct);									// Default initialization of SPIinit structure, SPI_Direction_2Lines_FullDuplex
	AppSPIinitStruct.SPI_Mode = SPI_Mode_Master;						// Override defaults
//	AppSPIinitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;	//9MHz(72/8), due to maximum rate for m95m01 is 16 MHz, but Fpclk/4=18MHz
	AppSPIinitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_256;
	// Here we have init structure for SPI
	
	// Init SPI using prepared init structure
	SPI_Init(AppSPI, &AppSPIinitStruct);
	SPI_SSOutputCmd(AppSPI, ENABLE);

}
/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
/*-- spiReadWrite - base subroutine to communicate with SPI slave------------------*/
/*-- SPI_TypeDef* SPIx - SPI device to use ----------------------------------------*/
/*-- uint8_t *rbuf - buffer to store received data to -----------------------------*/
/*-- uint8_t *tbuf - buffer to send data from -------------------------------------*/
/*-- int num - number of bytes to send or receive ---------------------------------*/
/*---------------------------------------------------------------------------------*/
/*-- CMD and ADDR to be put in tbuf as array --------------------------------------*/
/*-- data to write to be put in tbuf ----------------------------------------------*/
/*-- if tbuf is 0 then 'num' NOPs will be sent to receive 'num' bytes from slave --*/
/*-- if rbuf is 0 then no input will be placed to buffer --------------------------*/
/*---------------------------------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/
int spiReadWrite(SPI_TypeDef* SPIx, uint8_t *rbuf, const uint8_t *tbuf, int num)
{
	int i;
	for (i = 0; i < num; i++){
		if (tbuf) SPI_I2S_SendData(SPIx, tbuf[i]);
		else SPI_I2S_SendData(SPIx, NOP);

		while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);

		if (rbuf) rbuf[i] = SPI_I2S_ReceiveData(SPIx);
		else SPI_I2S_ReceiveData(SPIx);
	}
	return i;
}

/*---------------------------------------------------------------------------------*/
/*-- spiXByte - обмен байтами по SPI ----------------------------------------------*/
/*-- SPI_TypeDef* SPIx - SPI device to use ----------------------------------------*/
/*-- out - отсылаемый байт --------------------------------------------------------*/
/*-- полученный байт - в возвращаемом значении ------------------------------------*/
/*---------------------------------------------------------------------------------*/
uint8_t spiXByte(SPI_TypeDef* SPIx, uint8_t out)
{
	int j1 = 0, j2 = 0;
	uint8_t RX;
	SPI_I2S_SendData(SPIx, out);
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET) j1++;
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET) j2++;
	RX = SPI_I2S_ReceiveData(SPIx);
	return RX;
//	return RX = SPI_I2S_ReceiveData(SPIx);
}
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
/*---- INTERRUPT HANDLERS  --------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/
