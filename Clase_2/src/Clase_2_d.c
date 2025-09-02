/*
 * Considerando pulsadores normalmente abiertos conectados en un extremo a
 * masa y en el otro directamente a las entradas P0.0 y p0.1. Realizar un
 * programa que identifique en una variable cual o cuales pulsadores han sido
 * presionados. Las identificaciones posibles a implementar en esta variable van a
 * ser "ninguno", "pulsador 1", "pulsador 2", "pulsador 1 y 2".
 *
 * 	Created on: Aug 29, 2025
 *  Author: Matias Costamagna
 *
 */

#ifdef _USE_CMSIS
#include "LPC17xx.h"
#endif

#include <stdio.h>

#define INPUT_pins (3 << 0)

void cfgGPIO(void);

int main(void)
{
	const char *variable;

	cfgGPIO();

	while(1){
		uint32_t estado = LPC_GPIO0 -> FIOPIN & INPUT_pins;

		switch(estado){
			case 0x03:
				variable = "ninguno";
				break;
			case 0x02:
				variable = "pulsador 1";
				break;
			case 0x01:
				variable = "pulsador 2";
				break;
			case 0x00:
				variable = "pulsador 1 y 2";
				break;
		}
	}

	return 0;
}

void cfgGPIO(void)
{
	LPC_GPIO0 -> FIODIR &= ~INPUT_pins;

	LPC_PINCON -> PINMODE0 &= ~(0xF << 0); //Reset value, pull-up on P0.0 and P0.1
}
