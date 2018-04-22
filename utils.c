/*-------------------------------------------*/
/*  Miscellaneous utilities                  */
/*-------------------------------------------*/

#include "utils.h"
#include "stm32f10x_gpio.h"

static uint32_t TimingDelay;

/*********************************************/
/*********************************************/
void Delay(uint32_t nTime){
    TimingDelay = nTime;
    while(TimingDelay != 0);
}
/*-------------------------------------------*/

void __tdelay(uint8_t n) {
	for (int i = n; i; i--);
}
/*-------------------------------------------*/

void __strobe(void) {
	GPIO_WriteBit(__strobePort, __strobePin, Bit_SET);
	__tdelay(20);
	GPIO_WriteBit(__strobePort, __strobePin, Bit_RESET);
	__tdelay(20);
}


/*-------------------------------------------*/
/*---- INTERRUPT HANDLERS  ------------------*/
/*-------------------------------------------*/

void SysTick_Handler(void){
    if (TimingDelay != 0x00)
    TimingDelay --;
}
/*-------------------------------------------*/
