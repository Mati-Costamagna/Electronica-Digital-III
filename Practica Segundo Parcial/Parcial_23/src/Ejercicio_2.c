/*
 * Utilizando interrupciones por GPIO realizar un código en C que permita, mediante 4 pines de
 * entrada GPIO, leer y guardar un número compuesto por 4 bits. Dicho número puede ser
 * cambiado por un usuario mediante 4 switches, los cuales cuentan con sus respectivas
 * resistencias de pull up externas. El almacenamiento debe realizarse en una variable del tipo
 * array de forma tal que se asegure tener disponible siempre los últimos 10 números elegidos
 * por el usuario, garantizando además que el número ingresado más antiguo, de este conjunto
 * de 10, se encuentre en el elemento 9 y el número actual en el elemento 0 de dicho array. La
 * interrupción por GPIO empezará teniendo la máxima prioridad de interrupción posible y cada
 * 200 números ingresados deberá disminuir en 1 su prioridad hasta alcanzar la mínima posible.
 * Llegado este momento, el programa deshabilitará todo tipo de interrupciones producidas por
 * las entradas GPIO. Tener en cuenta que el código debe estar debidamente comentado.
 *
 * 	Created on: Sep 8, 2025
 *  Author: Matias Costamagna
 *
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>
#include <stdbool.h>

static uint32_t priority = 0;
static const uint32_t maxPriority = 31;
static uint32_t numbers[10] = {};

void cfgIntGPIO(void);

int main (void)
{
	cfgIntGPIO();

	while(1){};

	return 0;
}

void cfgIntGPIO(void)
{
	LPC_PINCON->PINMODE0 |= (0xAA << 0);
	LPC_GPIOINT->IO0IntEnF |= (0x0F << 0);

	NVIC_SetPriority(EINT3_IRQn, priority);
	NVIC_EnableIRQ(EINT3_IRQn);
}

void EINT3_IRQHandler(void)
{
	static uint32_t inputs = 0;
	inputs++;
	if(inputs == 200){
		inputs = 0;
		priority = (priority + 1) % maxPriority;
		if(priority == 0){
			NVIC_DisableIRQ(EINT3_IRQn);
		}else{
			NVIC_SetPriority(EINT3_IRQn, priority);
		}
	}

	for(uint8_t inte = 9; inte > 0; inte--){
		numbers[inte] = numbers [inte - 1];
	}
	numbers[0] = LPC_GPIO0->FIOPIN & 0x0F;
	LPC_GPIOINT->IO0IntClr |= (0x0F << 0);
}
