/*
 * Configurar la interrupción externa EINT1 para que interrumpa por flanco
 * de bajada y la interrupción EINT2 para que interrumpa por flanco de
 * subida. En la interrupción por flanco de bajada configurar el systick para
 * desbordar cada 25 mseg, mientras que en la interrupción por flanco de
 * subida configurarlo para que desborde cada 60 mseg. Considerar que
 * EINT1 tiene mayor prioridad que EINT2.
 *
 * 	Created on: Aug 29, 2025
 *  Author: Matias Costamagna
 *
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <stdio.h>

#define INT_BITS (3<<1)

uint32_t nTicks_1 = 0x26259F;
uint32_t nTicks_2 = 0x5B8D7F;
void cfgGPIO(void);
void cfgIntExt(void);
void cfgSysTick(void);


int main(void)
{
	cfgGPIO();

	cfgIntExt();

	cfgSysTick();

	while(1){};

	return 0;
}

void cfgGPIO(void)
{
	LPC_PINCON->PINSEL4 |= (0b0101<<22);
}

void cfgIntExt(void)
{
	LPC_SC->EXTMODE |= INT_BITS;
	LPC_SC->EXTOPOLAR &= ~(1<<1);
	LPC_SC->EXTOPOLAR |= (1<<2);
	LPC_SC->EXTINT |= INT_BITS;
	NVIC_EnableIRQ(EINT1_IRQn);
	NVIC_EnableIRQ(EINT2_IRQn);
	NVIC_SetPriority(EINT1_IRQn, 1);
	NVIC_SetPriority(EINT2_IRQn, 2);
}

void cfgSysTick(void)
{
	SysTick->LOAD = nTicks_1;
	SysTick->VAL = 0;
	SysTick->CTRL = 0x05;
}

void EINT1_IRQHandler(void)
{
	SysTick->LOAD = nTicks_1;
	SysTick->VAL = 0;
	LPC_SC->EXTINT |= (1<<1);
}

void EINT2_IRQHandler(void)
{
	SysTick->LOAD = nTicks_2;
	SysTick->VAL = 0;
	LPC_SC->EXTINT |= (1<<2);
}
