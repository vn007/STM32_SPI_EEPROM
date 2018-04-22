/*-------------------------------------------*/
/*  Header for miscellaneous utilities       */
/*-------------------------------------------*/

#ifndef __UTILS_H
#define __UTILS_H

#include "stdint.h"

#define STOP while(1);

/*********************************************/
/*********************************************/
void Delay(uint32_t nTime);
void __tdelay(uint8_t n);
void SysTick_Handler(void);
void __strobe(void);

#define __strobePort	GPIOA
#define __strobePin	GPIO_Pin_0


/*-------------------------------------------*/


#endif // __UTILS_H
