/*
 * Una famosa empresa de calzados a incorporado a sus zapatillas 10 luces leds
 * comandadas por un microcontrolador LPC1769 y ha pedido a su grupo de
 * ingenieros que diseñen 2 secuencias de luces que cada cierto tiempo se vayan
 * intercalando (secuencia A - secuencia B- secuencia A- ... ). Como todavía no se
 * ha definido la frecuencia a la cual va a funcionar el CPU del microcontrolador,
 * las funciones de retardos que se incorporen deben tener como parámetros de
 * entrada variables que permitan modificar el tiempo de retardo que se vaya a
 * utilizar finalmente. Se pide escribir el código que resuelva este pedido,
 * considerando que los leds se encuentran conectados en los puertos P0,0 al P0.9.
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

// TODO: insert other include files here

// TODO: insert other definitions and declarations here

#define LED_pins (1023 << 0)

void delay(uint32_t time);
void secuencia(uint32_t sequence_time, uint32_t cambio);

int main(void)
{

	uint32_t contador;
	uint32_t cambio;
	uint32_t time_A = 1000;
	uint32_t time_B = 2000;
	uint32_t switch_time = 50000;
	LPC_GPIO0 -> FIODIR |= LED_pins;
	LPC_GPIO0 -> FIOCLR |= LED_pins;

	while(1){
		cambio = switch_time/time_A;
		secuencia(time_A, cambio);
		cambio = switch_time/time_B;
		secuencia(time_B, cambio);
	}
    return 0 ;
}

void secuencia(uint32_t time, uint32_t cambio)
{
	for(contador = 0; contador < cambio; contador++){
		LPC_GPIO0 -> FIOSET |= LED_pins;
		delay(time);
		LPC_GPIO0 -> FIOCLR |= LED_pins;
		delay(time);
	}
	return;
}

void delay(uitn32_t time)
{
	uint32_t contador;
	for(contador = 0; contador < time; contador++){};
}
