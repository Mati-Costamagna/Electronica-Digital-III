/*
 * 1.- Configure el Systick Timer de modo que genere una forma de onda
 * llamada PWM. Esta señal debe ser extraida por el pin P0.22 para que
 * controle la intensidad de brillo del led. El periodo de la señal debe ser
 * de 10ms con un duty cycle de 10%. Configure la interrupción externa
 * EINT0 de modo que cada vez que se entre en una rutina de
 * interrupción externa el duty cycle incremente en un 10% (1ms). Esto se
 * repite hasta llegar al 100%, luego, si se entra nuevamente a la
 * interrupción externa, el duty cycle volverá al 10%.
 * 2.- Modificar los niveles de prioridad para que la interrupción por systick
 * tenga mayor prioridad que la interrupción externa.
 *
 * 	Created on: Sep 2, 2025
 *  Author: Matias Costamagna
 *
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif


#include <stdio.h>

volatile uint32_t nticks_dc;
uint32_t nticks_1ms = 99999;
uint32_t nticks_10ms = 999999999;

void cfgGPIO(void);
void cfgIntExt(void);

int main(void) {

	nticks_dc = nticks_1ms;

	cfgGPIO();

	cfgIntExt();

	cfgSysTick();

	while(1){};

    return 0 ;
}

void cfgGPIO(void)
{
	LPC_PINCON->PINSEL1 &= ~(3<<7);
	LPC_GPIO0->FIODIR |= (1<<22);
}

void cfgIntExt(void)
{
	LPC_SC->EXTMODE &= ~(1<<0);
	LPC_SC->EXTPOLAR &= ~(1<<0);
	LPC_SC->EXTINT |= (1<<0);
	NVIC_EnableIRQ(EINT0_IRQn);
	NVIC_SetPriority(EINT0_IRQn, 2);
}

void cfgSysTick(void)
{
	LPC_GPIO0->FIOSET |= (1<<22);
	SysTick->LOAD = nticks_1ms;
	SysTick->VAL = 0;
	SysTick->CTRL |= (7<<0);
	NVIC_SetPriority(SysTick_IRQn, 1);

void EINT0_IRQHandler(void)
{
	LPC_SC->EXTINT |= (1<<0);
	nticks_dc += nticks_1ms;
	if(nticks_dc > nticks_10ms){
		nticks_dc = nticks_1ms;
	}
}

void SysTick_IRQHandler(void)
{
	if(LPC_GPIO0->FIOPIN & (1<<22)){
		LPC_GPIO0->FIOCLR |= (1<<22);
		SysTick->LOAD = (nticks_10ms - nticks_dc);\
		SysTick->VAL = 0;
	}else{
		LPC_GPIO0->FIOSET |= (1<<22);
		SysTick->LOAD = nticks_dc;
		SysTick->VAL = 0;
	}
}
