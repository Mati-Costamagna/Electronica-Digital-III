/*
 * Realizar un programa que configure el puerto P2.0 y P2.1 para que
 * provoquen una interrupción por flanco de subida para el primer pin y
 * por flanco de bajada para el segundo. Cuando la interrupción sea por
 * P2.0 se enviará por el pin P0.0 la secuencia de bits 010011010. Si la
 * interrupción es por P2.1 se enviará por el pin P0.1 la secuencia
 * 011100110. Las secuencias se envían únicamente cuando se produce
 * una interrupción, en caso contrario la salida de los pines tienen valores
 * 1 lógicos. ¿que prioridad tienen configuradas por defecto estas
 * interrupciones?
 *
 * 	Created on: Sep 2, 2025
 *  Author: Matias Costamagna
 *
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>

#define IO_PINS (3 << 0)
void sequence_0(void);
void sequence_1(void);
void cfgGPIO(void);
void cfgIntGPIO(void);
void delay(void);

int main(void)
{
	cfgGPIO();

	cfgIntGPIO();

	while(1){
		LPC_GPIO0->FIOSET |= IO_PINS;
	}

	return 0;
}


void cfgGPIO(void)
{
	LPC_PINCON->PINSEL0 &= ~(15 << 0); //P0.0 y P0.1 como GPIO
	LPC_PINCON->PINSEL4 &= ~(15 << 0); //P2.0 y P2.1 como GPIO
	LPC_GPIO0->FIODIR |= IO_PINS;
	LPC_GPIO2->FIODIR &= ~IO_PINS;
}

void cfgIntGPIO(void)
{
	LPC_GPIOINT->IO2IntEnR |= (1<<0); //P2.0 flanco ascendente
	LPC_GPIOINT->IO2IntEnF |= (1<<1); //P2.1 flanco descendete
	LPC_GPIOINT->IO2IntClr |= IO_PINS;

	NVIC_EnableIRQ(EINT3_IRQn);
}

void EINT3_IRQHandler(void)
{
	if(LPC_GPIOINT->IntStatus & (1 << 2)){
		if(LPC_GPIOINT->IO2IntStatR & (1<<0)){
			LPC_GPIOINT->IO2IntClr = (1<<0);
			sequence_0();
		}else{
			LPC_GPIOINT->IO2IntClr = (1<<1);
			sequence_1();
		}
	}
}

void sequence_0(void)
{
	LPC_GPIO0->FIOCLR |= (1<<0);
	delay();
	LPC_GPIO0->FIOSET |= (1<<0);
	delay();
	LPC_GPIO0->FIOCLR |= (1<<0);
	delay();
	delay();
	LPC_GPIO0->FIOSET |= (1<<0);
	delay();
	delay();
	LPC_GPIO0->FIOCLR |= (1<<0);
	delay();
	LPC_GPIO0->FIOSET |= (1<<0);
	delay();
	LPC_GPIO0->FIOCLR |= (1<<0);
	delay();
}

void sequence_1(void)
{
	LPC_GPIO0->FIOCLR |= (1<<1);
	delay();
	LPC_GPIO0->FIOSET |= (1<<1);
	delay();
	delay();
	delay();
	LPC_GPIO0->FIOCLR |= (1<<1);
	delay();
	delay();
	LPC_GPIO0->FIOSET |= (1<<1);
	delay();
	delay();
	LPC_GPIO0->FIOCLR |= (1<<1);
	delay();
}

void delay(void)
{
	uint32_t time = 10000;
	uint32_t contador;

	for(contador = 0; contador < time; contador++){};

}
