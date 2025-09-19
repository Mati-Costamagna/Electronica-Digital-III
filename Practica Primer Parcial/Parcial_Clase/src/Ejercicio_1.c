/*
 * En la dirección de memoria 0x20080000 se tiene almacenado un valor de 32 bits que
 * representa cuatro formas de onda binarias de 8 bits cada una. Utilizando los registros
 * de configuración y el systick del microcontrolador LPC1769 generar por el pin P2.8 las
 * formas de onda binarias en serie de 8 bits almacenadas en la dirección anteriormente
 * mencionada, y generar por el puerto P2 el promedio de la forma de onda binaria
 * seleccionada.
 * El pin asociado a EINT0 presenta una resistencia de pull-down externa.
 * Configurar la interrupción de dicho pin con prioridad 3, para que, cada vez que interrumpa,
 * termine la forma de onda actual y cambie a la siguiente (una vez que llega a la última,
 * debe volver a comenzar la primera).
 * El periodo de la señal debe ser establecido mediante la interrupción (prioridad 2) del pin
 * asociado a EINT1, el cual presenta una resistencia de pull-up. De manera que se pueda
 * cambiar el periodo de la señal entre 80[ms] (por defecto) y 160[ms]. Considerando un
 * CCLK de 65[MHz].
 *
 * 	Created on: Sep 4, 2025
 *  Author: Matias Costamagna
 *
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>

static const uint8_t size_wave = 8;
static const uint32_t starting_idx = 0x20080000;
static const uint32_t ticks_10ms = 64999;
static const uint32_t ticks_20ms = 129999;
static uint32_t *ptr = (uint32_t*) starting_idx;
static float avg = 0;
void cfgGPIO(void);
void cfgIntExt(void);
void cfgSysTick(void);

int main(void)
{
	cfgGPIO();

	cfgIntExt();

	cfgSysTick(ticks80_ms);

	while(1){
		avg = calcAverage();
		showAverage(avg);
	}
	return 0;
}

void cfgGPIO(void)
{
	LPC_GPIO2->FIODIR |= (1<<8);
	LPC_GPIO2->FIODIR |= ((256 << 0) - 1);

	LPC_PINCON->PINSEL4 |= ((1<<20) | (1<<22));
	LPC_PINCON->PINMODE4 |= ((2<<20) | (1<<22));
}

void cfgIntExt(void)
{
	LPC_SC->EXTINT |= (3<<0);
	LPC_SC->EXTPOLAR |= (1 << 0) &= ~(1<<1);
	NVIC_SetPriority(EINT0_IRQn, 3);
	NVIC_EnableIRQ(EINT0_IRQn);
	NVIC_EnableIRQ(EINT1_IRQn);
}

void cfgSysTick(uint32_t ticks)
{
	SysTick->LOAD = ticks;
	SysTick->VAL = 0;
	SysTick->CTRL = (7 << 0);
}

void EINT1_IRQHandler(void)
{
	static uint32_t vTicks = 0;
	vTicks++;
	if(vTicks % 2){
		cfgSysTick(ticks_20ms);
	}else{
		cfgSysTick(ticks_10ms);
	}
	LPC_SC->EXTINT |= (1<<1);
}

void EINT0_IRQHandler(void)
{
	static uint32_t aux = 0;
	aux = (aux+1)%4;
	ptr = (uint32_t*)(starting_idx + size_wave*aux);
	LPC_SC->EXTINT |= (1<<0);
}

void SysTick_Handler(void)
{
	static uint32_t selectedBit = 0;
	LPC_GPIO2->FIOCLR |= (*ptr & (1<<selectedBit));
	selectedBit = (selectedBit+1)%size_wave;
	LPC_GPIO2->FIOSET |= (*ptr & (1<<selectedBit));
	SysTick->CTRL &= SysTick->CTRL;
}

float calcAverage(void)
{
	uint8_t sum = 0;
	for(uint8_t i = 0; i < size_wave; i++){
		sum += (*ptr & (1<<i));
	}
	return sum/size_wave;
}

void showAverage(float avg){
	uint8_t avg8b = (uint8_t)(avg * 255);
	LPC_GPIO2->FIOCLR &= ~(0xFF);        // limpiar P2[7:0]
	LPC_GPIO2->FIOSET |= (avg8b & 0xFF); // escribir nuevo valor en P2[7:0]

}
