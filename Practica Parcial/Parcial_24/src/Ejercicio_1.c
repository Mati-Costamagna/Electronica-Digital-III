/*
 * Un estacionamiento automatizado utiliza una barrera que se abre y cierra en funcion de la
 * validacion de un ticket de acceso utilizando un LPC1769 Rev. D trabajando a una frecuencia
 * de CCLK a 70[MHz].
 *
 * Cuando el sistema detecta que un automovil se ha posicionado frente a la barrera, se debe
 * activar un sensor conectado al pin P2[10] mediante una interrupcion externa (EINT). Una vez
 * validado el ticket, el sistema activa un motor que abre la barrera usando el pin P0[15]. El
 * motor debe estar activado por X segundos y luego apagarse, utilizando el temporizador
 * SysTick para contar el timepo. Si el ticket es invalido, se encendera un LED rojo conectado
 * al pin P1[4].
 *
 * Para gestionar el timepo de apertura de la abra, existe un switch conectado al pin P2[11]
 * que dispone de una ventana de configuracion de 3 segundos gestionada por el temporizador
 * SysTick.
 *
 * Durante dicha ventana, se debe contar cuantas veces se presiona el switch y en funcion de
 * dicha cantidad, establecer el tiempo de la barrera.
 *
 * Cantidad de veces que se presiona el switch 	| Tiempo de barrera
 * 			0x00								| 		5 seg
 * 			0x01								|		10 seg
 * 			0x02								|		20 seg
 * 			0x03								|	   	40 seg
 * 			0x04								|		5 seg
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
#include <stdbool.h>

const static uint32_t ticks_200ms = 13999999; // 70e6 * 0.2 - 1
const static uint32_t overflow_3s = 15;
const static uint32_t overflow_5s = 25;
const static uint32_t overflow_10s = 50;
const static uint32_t overflow_20s = 100;
const static uint32_t overflow_40s = 200;
static uint32_t overflows = 0;
static uint32_t overflows_goal = 0;
static uint8_t switch_count = 0;


int main(void)
{

	cfgGPIO();

	cfgIntExt();

	while(1){};

    return 0 ;
}

void cfgGPIO(void)
{
	LPC_GPIO0->FIODIR |= (1<<15); //P1.15 para el motor
	LPC_GPIO1->FIODIR |= (1<<4); //P1.4 para el LED rojo
}

void cfgIntExt(void)
{
	LPC_PINCON->PINSEL4 |= (0b0101 << 20); //P2.10 y P2.11 para interrupciones
	LPC_SC->EXTINT |= (3<<0); //EINT0 y EINT1
	NVIC_EnableIRQ(EINT0_IRQn)
}

void cfgSysTick(void)
{
	SysTick->LOAD = ticks_200ms;
	SysTick->VAL = 0;
	SysTick->CTRL = (7<<0);
}

uint32_t switchCounter(uint8_t switch_c)
{
	switch(switch_c){          // 0..3, y 4 → 0 por tabla
		case 0:  return overflow_5s;
		case 1:  return overflow_10s;
		case 2:  return overflow_20s;
		case 3:  return overflow_40s;
		case 4:  return overflow_5s;
		default: return overflow_5s;
	}
}


void EINT0_IRQHandler(void)
{
	if(tiketValido){
		cfgSysTick();
		overflows_goal = overflow_3s;
		NVIC_EnableIRQ(EINT1_IRQn);
	}else{
		LPC_GPIO1->FIOSET |= (1<<4);
	}
	LPC_SC->EXTINT |= (1<<0);
}

void EINT1_IRQHandler(void)
{
	switch_count++;
	LPC_SC->EXTINT |= (1<<1);
}

void SysTick_Handler(void){
	overflows++;
	if(overflows == overflows_goal){
		if(overflows_goal == overflow_3s){
			overflows_goal = switchCounter(switch_count);
			switch_count = 0;
			LPC_GPIO0->FIOSET |= (1<<15);
		}else{
			LPC_GPIO0->FIOCLR |= (1<<15);
			SysTick->CTRL = 0;
		}
	}
	SysTick->CTRL &= SysTick->CTRL;
}
