/*
 * En una fabrica, hay un sistema de alarma utilizando una LPC1769 Rev. D trabajando a una
 * frecuencia de CCLK de 100 [MHz], conectado a un sensor de puerta que se activa cuando
 * la puerta se abre. El sensor esta conectado al pin P0[6], el cual genera una interrupcion
 * externa (EINT) cuando se detecta una apertura (cambio de estado). Al detectar que la
 * puerta se ha abierto, el sistema debe iniciar un temporizador utilizando el Systick para
 * contar un periodo de 30 segundos.
 *
 * Durante estos 30 segundos, el usuario debera introducir un codigo de desactivacion
 * mediante un DIP switch de 4 entradas conectado a los pines P2[0]-P2[3]. El codigo correcto
 * es 0xAA (1010 en binario). El usuario tiene dos intentos para introducir el codigo correcto.
 * Si despues de dos intentos el codigo ingresado es incorrecto, la alarma se activara,
 * encendiendo un buzzer conectado al pin P1[10].
 *
 * 	Created on: Sep 6, 2025
 *  Author: Matias Costamagna
 *
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>
#include <stdbool.h>

#define SYST_150MS_LOAD  (1499999) // 70e6 * 0.2 - 1

void cfgGPIO(void);
void cfgIntExt(void);
void cfgSysTick(void);

static const uint32_t overflow_15s = 100;
static uint32_t intentos = 0;
static uint32_t overflow_count = 0;

int main(void)
{
	cfgGPIO();

	cfgIntExt();

	while(1){};

    return 0 ;
}

void cfgGPIO(void)
{
	LPC_GPIO1->FIODIR |= (1<<10); // P1.10 salida
	LPC_GPIO1->FIOCLR |= (1<<10);
	LPC_GPIO2->FIODIR &= ~(0b1111<<0); // P2.0 - P2.3 como entradas
}

void cfgIntExt(void)
{
	LPC_PINCON->PINSEL4 |= (1<<20);
	LPC_SC->EXTINT |= (1<<0);
	LPC_SC->EXTPOLAR |= (1<<0);
	NVIC_EnableIRQ(EINT0_IRQn)
}

void cfgSysTick(void)
{
	SysTick->LOAD = nTicks;
	SysTick->VAL = 0;
	SysTick->CTRL = (7<<0);
}

void EINT0_IRQHandler(void)
{
	cfgSysTick();
	LPC_SC->EXTINT |= (1<<0);
}

void SysTick_Handler(void)
{
	overflow_count++;
	if(overflow_count == overflow_15s){
		if((LPC_GPIO2->FIOPIN & 0b1111) == 0b1010){
			SysTick->CTRL = (4 << 0); //Deshabilito la cuenta
		}else{
			intentos++;
			if(intentos == 2){
				LPC_GPIO1->FIOSET |= (1<<10);
				SysTick->CTRL = (4 << 0); //Deshabilito la cuenta
			}
		}
	}
	SysTick->CTRL &= SysTick->CTRL;
}
