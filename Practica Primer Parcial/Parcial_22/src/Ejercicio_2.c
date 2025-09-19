/*
 * Utilizando interrupciones por SysTick y por eventos externos EINT, realizar un programa que permita habilitar y
 * deshabilitar el temporizador por el flanco ascendente en el pin P2.11. El temporizador debe desbordar cada 10 ms
 * utilizando un reloj de core CCLK = 62 [MHz]. Por cada interrupcion del SysTick, se debe mostrar por el puerto P0 el
 * promedio de los valores guardados en la variable "uint8_t values[8]". Se pide ademas detallar los calculos realizados
 * para obtener el valor a cargar en el registro RELOAD y asegurar que la interrupcion por Systick sea mayor que la
 * prioridad de la interrupcion del evento externo. El codigo debe estar debidamente comentado.
 *
 * 	Created on: Sep 9, 2025
 *  Author: Matias Costamagna
 *
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>

static uint8_t values[8] = {};
static const uint32_t nTicks = 619999; // nTicks = 60e6 * 10e-3 - 1

void cfgGPIO(void);
void cfgIntExt(void);
void cfgSysTick(void);
uint8_t averatge(void);

int main(void)
{
	cfgGPIO();

	cfgIntExt();

	while(1){};

	return 0;
}

void cfgGPIO(void)
{
	LPC_GPIO0->FIODIR |= (0xFF << 0);
}

void cfgIntExt(void)
{
	LPC_PINCON->PINSEL4 |= (1<<22); //EINT1

	LPC_SC -> EXTMODE |= (1<<1); //Por flanco
	LPC_SC -> EXTPOLAR |= (1<<1); //Flanco de subida

	NVIC_SetPriority(EINT1_IRQn, 2);
	NVIC_EnableIRQ(EINT1_IRQn);
}

void cfgSysTick(void)
{
	SysTick->LOAD = nTicks;
	SysTick->VAL = 0;
	SysTick->CTRL = (7<<0); // ENABLE = 1 | TICKINT = 1 | CLKSOURCE = 1
}

void EINT1_IRQHandler(void)
{
	static uint32_t vTicks = 0;
	vTicks++;
	if(vTicks % 2){
		cfgSysTick();
	}else{
		SysTick->CTRL = (4<<0); // ENABLE = 0 | TICKINT = 0
	}

	LPC_SC->EXTINT |= (1<<1);
}

void SysTick_Handler(void)
{
	LPC_GPIO0->FIOCLR |= (0xFF << 0);
	LPC_GPIO0->FIOSET |= average();

	SysTick->CTRL &= SysTick->CTRL;
}

uint8_t average(void)
{
	uint8_t sum = 0;
	uint8_t avg = 0;

	for(uint8_t inte = 0; inte < 8; inte++){
		sum += values[inte];
	}

	avg = sum / 8;

	return avg;
}
