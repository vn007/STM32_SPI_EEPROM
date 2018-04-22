/*-------------------------------------------*/
/*  EEPROM SPI utilities                     */
/*-------------------------------------------*/

#include "eeprom.h"

/*********************************************/

/*---------------------------------------------------------------------------------*/
/*-- eepromUCMD - send UniByte command --------------------------------------------*/
/*-- SPIx - SPI device to use -----------------------------------------------------*/
/*-- cmd - buffer to store received data ------------------------------------------*/
/*---------------------------------------------------------------------------------*/
void eepromUCMD(SPI_TypeDef *SPIx, uint8_t cmd){
	eepromSelect();
	
	spiXByte(SPIx, cmd);
	
	__tdelay(20);
	eepromDeselect();
}
/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
/*-- eepromRW_Array - запись или чтение массива -----------------------------------*/
/*-- разбиение массива на страницы и постраничная запись --------------------------*/
/*-- SPIx - SPI device to use -----------------------------------------------------*/
/*-- cmd - команда, WRITE | READ --------------------------------------------------*/
/*-- addr0 - начальный адрес в eeprom ---------------------------------------------*/
/*-- buf - массив, из которого производится запись --------------------------------*/
/*-- length - длина массива, число записываемых байт ------------------------------*/
/*---------------------------------------------------------------------------------*/
int eepromRW_Array(SPI_TypeDef *SPIx, uint8_t cmd, int addr0, uint8_t *buf, int length)
{
	t_pagination tpg = __pagination(addr0, EEPROM_PAGE_SIZE, length);
	
	// write cycle, first (maybe) incomplete page
	eepromRW_PG(AppSPI, cmd, addr0, buf, tpg.n0);		// Read/Write 0-th page
	
	int i;
	for (i = 0; i < tpg.np; i++) {
		eepromRW_PG(AppSPI, cmd, addr0 + tpg.n0 + i * EEPROM_PAGE_SIZE,	// Read/Write sequent pages that are full
					buf + tpg.n0 + i * EEPROM_PAGE_SIZE,				// Read/Write sequent pages that are full
					EEPROM_PAGE_SIZE);									// Read/Write sequent pages that are full
	}

	if (tpg.ne) {
		int last_page_offset = tpg.n0 + tpg.np * EEPROM_PAGE_SIZE;
		eepromRW_PG(AppSPI, cmd, addr0 + last_page_offset,				// Read/Write sequent pages
		   		    buf + last_page_offset,	tpg.ne);					// Read/Write sequent pages
	}

	return tpg.n0 + i * tpg.np * EEPROM_PAGE_SIZE + tpg.ne;
}
/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
/*-- eepromRW_PG - запись или чтение внутри одной страницы 'num' байт без анализа -*/
/*-- SPI_TypeDef* SPIx - SPI device to use ----------------------------------------*/
/*-- uint32_t addr - начальный адрес ----------------------------------------------*/
/*-- uint8_t *buf - массив, из которого производится запись -----------------------*/
/*-- int num - число записываемых байт --------------------------------------------*/
/*---------------------------------------------------------------------------------*/
int eepromRW_PG(SPI_TypeDef *SPIx, uint8_t cmd, uint32_t addr, uint8_t *buf, uint32_t num)
{
	if (cmd == WRITE) eepromUCMD(AppSPI, WREN);

	eepromSelect();

	spiXByte(SPIx, cmd);	// sending command itself
	
	// parsing and sending address, MSB first
	uint32_t _addr;

	int i;
	for (i = EEPROM_ADDRESS_BYTES - 1; i >= 0; i--){	// '2' - for 24-bit address, '1' - for 16-bit address
		_addr = addr;
		_addr >>= i*8;
		_addr &= 0xFF;
		spiXByte(SPIx, _addr);		// sending address byte
	}

	for (i = 0; i < num; i++) {
		if (cmd == WRITE) spiXByte(SPIx, buf[i]);		// sending bytes from array 'buf'
		else if (cmd == READ) buf[i] = spiXByte(SPIx, NOP);		// getting bytes from EEPROM and store it to array 'buf'
	}
	
	eepromDeselect();
	
//	Delay(5);		// tW, see datasheet
	if (cmd == WRITE) {
__strob();
		while (eepromReadStatus(AppSPI) & WIP);			// testing WIP - 'Write In Progress' of EEPROM (empirically -> up to 426 cycles)
__strob();
	}
	
	return i;
}
/*---------------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------------*/
/*-- pagination - разбиение массива на страницы -----------------------------------*/
/*-- addr0 - начальный адрес считывания/записи в EEPROM ---------------------------*/
/*-- pgsize - размер страницы в EEPROM --------------------------------------------*/
/*-- length - длина записываемого/считываемого массива ----------------------------*/
/*---------------------------------------------------------------------------------*/
t_pagination __pagination(int addr0, int pgsize, int length)
{
	t_pagination tpg;
	int __n0;
	__n0 = pgsize - addr0 % pgsize;					// число байт в (возможно) неполной 0-й странице
	tpg.n0 = (length < __n0? length: __n0);
	tpg.np = (length - tpg.n0) / pgsize;			// число полных страниц
	tpg.ne = length - tpg.n0 - tpg.np * pgsize;		// число байт в неполной последней странице
	
	return tpg;
}
/*---------------------------------------------------------------------------------*/

uint8_t eepromReadStatus(SPI_TypeDef *SPIx)
{
	uint8_t stat;

	eepromSelect();				// falling edge on Chip Select (S)
	
__tdelay(100);

	SPI_I2S_SendData(SPIx, RDSR);
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET);
	SPI_I2S_ReceiveData(SPIx);		// to flush receive buffer

	SPI_I2S_SendData(SPIx, NOP);	// to make clocks to read data that came to MISO
	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_BSY) == SET); 		//waiting for tx finished

	while (SPI_I2S_GetFlagStatus(SPIx, SPI_I2S_FLAG_RXNE) == RESET);	//waiting for rx filled

	stat = SPI_I2S_ReceiveData(SPIx);

__tdelay(100);
	eepromDeselect();

	return stat;
}
/*---------------------------------------------------------------------------------*/

void eepromSelect(void) {
	
	GPIO_WriteBit(EEPROM_PORT, EEPROM_CS_PIN, Bit_RESET);

}
/*---------------------------------------------------------------------------------*/

void eepromDeselect(void) {
	
	GPIO_WriteBit(EEPROM_PORT, EEPROM_CS_PIN, Bit_SET);

}
/*---------------------------------------------------------------------------------*/

void eepromHOLD(uint8_t bitstate) {
	
	GPIO_WriteBit(EEPROM_PORT, EEPROM_HOLD_PIN, (bitstate) ? Bit_SET : Bit_RESET);

}
/*---------------------------------------------------------------------------------*/

void eepromWrightProtect(uint8_t bitstate) {
	
	GPIO_WriteBit(EEPROM_PORT, EEPROM_W_PIN, (bitstate) ? Bit_SET : Bit_RESET);

}
/*---------------------------------------------------------------------------------*/

void eepromInit(void){
	eepromHOLD(SET);
	eepromWrightProtect(SET);
}
/*---------------------------------------------------------------------------------*/


/*---------------------------------------------------------------------------------*/
/*---- INTERRUPT HANDLERS  --------------------------------------------------------*/
/*---------------------------------------------------------------------------------*/
