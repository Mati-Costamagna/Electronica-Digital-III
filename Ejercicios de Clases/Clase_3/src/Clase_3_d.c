/*
 * Escribir un programa en C que permita sacar por los pines P1.16 al
 * P1.23 el equivalente en ascii de "a" si la interrupción se ha realizado
 * por interrupción de EINT1 y no hay interrupción pendiente de EINT0.
 * Para el caso en el cual se produzca una interrupción por EINT1 y exista
 * una interrupción pendiente de EINT0 sacar por el puerto el equivalente
 * en ascii de "A". La interrupción de EINT1 es por el flanco de subida
 * producido por un pulsador identificado como "tecla a/A", mientras que la
 * interrupción EINT0 es por el nivel alto de un pulsador etiquetado como
 * "Activación de Mayusculas".
 * Nota: Valerse de la configuración de los niveles de prioridad para que la
 * pulsación conjunta de "Activación de Mayúsculas" con "tecla a/A" de
 * como resultado a la salida el equivalente en ascii "A".
 *
 * 	Created on: Sep 2, 2025
 *  Author: Matias Costamagna
 *
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <stdio.h>

uint32_t ascii_a = 97;
uint32_t ascii_A = 65;

#define OUTPUT_PINS (255 << 16)


void cfgGPIO(void);
void cfgIntExt(void);

int main(void)
{
	cfgGPIO();

	cfgIntExt();

	while(1){};

	return 0;
}

void cfgGPIO(void)
{
	LPC_GPIO1->FIODIR |= OUTPUT_PINS; // P1.16 a P1.23 as output
}

void cfgIntExt(void)
{
	LPC_SC->EXTMODE &= ~(1<<0);
	LPC_SC->EXTMODE |= (1<<1);
	LPC_SC->EXTOPOLAR &= ~(1<<0);
	LPC_SC->EXTOPOLAR |= (1<<1);
	LPC_SC->EXTINT |= (3<<0);
	NVIC_EnableIRQ(EINT0_IRQn);
	NVIC_EnableIRQ(EINT1_IRQn);
	NVIC_SetPriority(EINT0_IRQn, 2);
	NVIC_SetPriority(EINT1_IRQn, 1);
}

void EINT1_IRQHandler(void)
{
	if (LPC_SC->EXTINT & (1<<0)){
		LPC_GPIO1->FIOPIN = (ascii_A << 16);
	}else{
		LPC_GPIO1->FIOPIN = (ascii_a << 16);
	}

	LPC_SC->EXTINT = (1<<1);
}
