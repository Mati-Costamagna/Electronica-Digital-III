/*
 * Consigna:
 * Configurar el pin P0.4 como entrada digital con resistencia de pull down y
 * utilizarlo para decidir si el valor representado por los pines P0.0 al P0.3 van a
 * ser sumados o restados al valor guardado en la variable "acumulador".
 * El valor inicial de "acumulador" es 0.
 *
 *  Created on: Aug 29, 2025
 *  Author: Matias Costamagna
 *
 */


#ifdef _USE_CMSIS
#include "LPC17xx.h"
#endif

#include <stdio.h>

#define INPUT_pins (15 << 0)
#define SELECTION_pin (1 << 4)

void cfgGPIO(void);

int main(void)
{
	uint32_t acumulador = 0;

	cfgGPIO();

	while(1){
		uint32_valor = (LPC_GPIO0->FIOPIN * INPUT_pins) >> 0;

		if(LPC_GPIO0 -> FIOPIN & SELECTION_pin){
			acumulador += valor;
		}else{
			acumulador -= valor;
		}
	}

	return 0;
}

void cfgGPIO(void)
{
	LPC_GPIO0 -> FIODIR &= ~(SELECTION_pin | INPUT_pins); //P0.0 to P0.4 as input

	LPC_PINCON -> PINMODE0 &= (0b11 << 8); //Limpio bits [9:8]
	LPC_PINCON -> PINMODE0 |= (0b11 << 8); //Pines [9:8] con pull-down interna

}
