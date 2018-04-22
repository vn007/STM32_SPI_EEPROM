/*-------------------------------------------*/
/*  Header for EEPROM SPI utilities          */
/* заголовок EEPROM утилит----------------*/
/*-------------------------------------------*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __EEPROM_H
#define __EEPROM_H

/*-------------------------------------------*/
#include "spi.h"
#include "utils.h"


/*-------------------------------------------*/
#define EEPROM_PORT GPIOA
#define EEPROM_W_PIN GPIO_Pin_8
#define EEPROM_HOLD_PIN GPIO_Pin_9
#define EEPROM_CS_PIN GPIO_Pin_10

#define EEPROM_SIZE				8192
#define EEPROM_PAGE_SIZE		32
#define EEPROM_ADDRESS_BYTES	2

/*-------------------------------------------*/
extern uint8_t xbuf[EEPROM_SIZE];

/*********************************************/
/*********************************************/
typedef enum eepromCMD {
	NOP		=	0x00,	// NO OPERATION //<--LV
	WREN	=	0x06,	// Write Enable
	WRDI	=	0x04,	// Write Disable
	RDSR	=	0x05,	// Read Status Register
	WRSR	=	0x01,	// Write Status Register
	READ	=	0x03,	// Read from Memory Array
	WRITE	=	0x02,	// Write to Memory Array
// Commands below are for -DF models only
	RDID	=	0x83,	// Read Identification Page
	WRID	=	0x82,	// Write Identification Page
	RDLS	=	0x83,	// Reads the Identification Page lock status
	LID		=	0x82	// Locks the Identification Page in read-only mode
} t_eepromCMD;

typedef enum eepromSTAT{
	WIP		=	0x01,	// Write In Progress
	WEL		=	0x02,	// Write Enable Latch
	BP0		=	0x04,	// Block Protet0
	BP1		=	0x08,	// Block Protet1
	SRWD	=	0x80	// Status Register Write Disable
} t_eepromSTAT;

typedef struct{
	int n0;				// число байт в (возможно) неполной 0-й странице
	int np;				// число полных страниц
	int ne;				// число байт в (возможно) неполной последней странице
} t_pagination;

extern t_eepromCMD eepromCMD;

int eepromWriteArray_NEW(SPI_TypeDef *SPIx, int addr0, uint8_t *buf, int length);
int eepromRW_PG(SPI_TypeDef *SPIx, uint8_t cmd, uint32_t addr, uint8_t *buf, uint32_t num);
int eepromRW_Array(SPI_TypeDef *SPIx, uint8_t cmd, int addr0, uint8_t *buf, int length);


uint8_t eepromReadStatus(SPI_TypeDef *SPIx);
t_pagination __pagination(int addr0, int pgsize, int length);

void eepromUCMD(SPI_TypeDef *SPIx, uint8_t cmd);
void eepromSelect(void);
void eepromDeselect(void);
void eepromInit(void);

/*-------------------------------------------*/
#endif // __EEPROM_H
