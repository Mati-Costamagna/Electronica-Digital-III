/*
 * Consigna:
 * En los pines P2.0 a P2.7 se encuentra conectado un display de 7 segmentos.
 * Utilizando la variable numDisplay [10] ={0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D,
 * 0x7D, 0x07, 0x7F, 0x67} que codifica los números del 0 a 9 para ser mostrados
 * en el display, realizar un programa que muestre indefinidamente la cuenta de 0
 * a 9 en dicho display.
 *
 * 	Created on: Aug 29, 2025
 *  Author: Matias Costamagna
 *
 */

#ifdef _USE_CMSIS
#include "LPC17xx.h"
#endif

#include <stdio.h>

#define LED_pins (255 << 0)

void cfgGPIO(void);
void delay(uint32_t time);

int main (void)
{
	uint32_t numDisplay[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};
	uint32_t counter = 0;
	uint32_t total_pins = 10;
	uint32_t delay_time = 10000;

	cfgGPIO();

	while(1){
		for(contador = 0; contador < 10; contador++){
			LPC_GPIO2 -> FIOCLR |= LED_pins;
			delay(delay_time);
			LPC_GPIO2 -> FIOSET = numDisplay[contador];
			delay(delay_time);
		}
	}

	return 0;
}

void cfgGPIO(void)
{
	LPC_GPIO2 -> FIODIR |= LED_pins;
}

void delay(uint32_t time)
{
	uint32_t contador;
	for(contador = 0; contador < time; contador++){};
}
