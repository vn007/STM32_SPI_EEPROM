/**
  ******************************************************************************
  * @file    SPI_EEPROM/main.c 
  * @author  LV
  * @date    01-April-2018
  ******************************************************************************
*/

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "stm32f10x_conf.h"

#include "spi.h"
#include "eeprom.h"
	  
#include "utils.h"

/* Private typedef -----------------------------------------------------------*/
typedef enum {FAILED = 0, PASSED = !FAILED} TestStatus;

/* Private define ------------------------------------------------------------*/

/* Global vars declaration ---------------------------------------------------*/
SPI_TypeDef *AppSPI;
SPI_InitTypeDef AppSPIinitStruct;
GPIO_TypeDef *AppGPIO;
GPIO_TypeDef *GPIOzz;
GPIO_InitTypeDef App_GPIO_initStruct;

uint8_t xbuf[8192];

extern void AppSPI_Init(void);

/*********************************************/
/*********************************************/
int main(void)
{
//	uint8_t Stat;

	AppGPIO = GPIOA;
	GPIOzz = GPIOB;
	
    if (SysTick_Config(SystemCoreClock / 1000)) while (1);	// To initialize SysTick
	Delay (5); // задержка Xms
	
	AppSPI = SPI1;				//for further addressing as certain application SPI name

	// Initialize the SPI module
	AppSPI_Init();
	
	eepromDeselect();		// "after power-up, a falling edge is required on Chip Select (S) before any
							// instructions can be started" - datasheet
	
//	Delay (1); // задержка Xms
	GPIO_WriteBit(AppGPIO, (EEPROM_HOLD_PIN | EEPROM_W_PIN), Bit_SET);		// deactivates HOLD and W

	SPI_Cmd(AppSPI, ENABLE);	// SPI switching On
	
__strob();

	int test_addr0 = 0;
	int test_length = EEPROM_SIZE;

	for (int i = 0; i < test_length; i++) {
		xbuf[i] = i;
	}

	eepromRW_Array(AppSPI, WRITE, test_addr0, xbuf, test_length);

	for (int i = 0; i < test_length; i++) {
		xbuf[i] = 0;
	}

	eepromRW_Array(AppSPI, READ, test_addr0, xbuf, test_length);

STOP	// for debugging purposes (see utils.h)
}

