/*
 * Realizar un programa que configure el puerto P0.0 y P2.0 para que
 * provoquen una interrupción por flanco de subida. Si la interrupción es
 * por P0.0 guardar el valor binario 100111 en la variable "auxiliar", si es
 * por P2.0 guardar el valor binario 111001011010110.
 *
 * 	Created on: Aug 29, 2025
 *  Author: Matias Costamagna
 *
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>

volatile uint32_t auxiliar;

void cfgGPIO(void);
void cfgIntGPIO(void);

int main(void)
{

	cfgGPIO();

	cfgIntGPIO();

	while(1){

	}

	return 0;
}


void cfgGPIO(void)
{
	LPC_PINCON->PINSEL0 &= ~(3<<0); //P0.0 como GPIO
	LPC_PINCON->PINSEL4 &= ~(3<<0); //P2.0 como GPIO
	LPC_GPIO0->FIODIR &= ~(1<<0);
	LPC_GPIO2->FIODIR &= ~(1<<0);
}

void cfgIntGPIO(void)
{
	LPC_GPIOINT->IO0IntEnR |= (1<<0); //P0.0 flanco ascendente
	LPC_GPIOINT->IO2IntEnR |= (1<<0); //P2.0 flanco ascendente
	LPC_GPIOINT->IO0IntClr |= (1<<0);
	LPC_GPIOINT->IO2IntClr |= (1<<0);

	NVIC_EnableIRQ(EINT3_IRQn);
}

void EINT3_IRQHandler(void)
{
	if(LPC_GPIOINT->IntStatus & (1 << 2)){
		if(LPC_GPIOINT->IO2IntStatR & (1<<0)){
			auxiliar = 0b111001011010110;
			LPC_GPIOINT->IO2IntClr = (1<<0);
		}
	}else if(LPC_GPIOINT->IntStatus & (1 << 0)){
		if(LPC_GPIOINT->IO0IntStatR & (1<<0)){
			auxiliar = 0b100111;
			LPC_GPIOINT->IO0IntClr = (1<<0);
		}
	}
}
