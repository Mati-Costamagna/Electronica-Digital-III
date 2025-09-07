/*
 * Utilizando Systick e interrupciones externas escribir un código en C que cuente indefinidamente de 0
 * a 9. Un pulsador conectado a Eint0 reiniciará la cuenta a 0 y se mantendrá en ese valor mientras el
 * pulsador se encuentre presionado. Un pulsador conectado a Eint1 permitirá detener o continuar la
 * cuenta cada vez que sea presionado. Un pulsador conectado a Eint2 permitirá modificar la velocidad
 * de incremento del contador. En este sentido, cada vez que se presione ese pulsador el contador pasará
 * a incrementar su cuenta de cada 1 segundo a cada 1 milisegundo y viceversa. Considerar que el
 * microcontrolador se encuentra funcionando con un reloj (cclk) de 16 Mhz. El código debe estar
 * debidamente comentado y los cálculos realizados claramente expresados. En la siguiente figura se
 * muestra una tabla que codifica el display y el esquema del hardware sobre el que funcionará el
 * programa.
 *
 *
 * 	Created on: Sep 7, 2025
 *  Author: Matias Costamagna
 *
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>
#include <stdbool.h>

static const uint32_t code[10] = {0x3F, 0x06, 0x5B, 0x4F, 0x66, 0x6D, 0x7D, 0x07, 0x7F, 0x67};
static uint8_t number = 0;
static uint32_t overflows_goal = 1000;
static bool status = 0;

void cfgGPIO(void);
void cfgIntExt(void);
void cfgSysTick(void);


int main(void)
{

	cfgGPIO();

	cfgIntExt();

	cfgSysTick();

	while(1){};

    return 0 ;
}

void cfgGPIO(void)
{
	LPC_GPIO0->FIODIR |= (0x7F << 0);
	LPC_GPIO0->FIOCLR |= (0x7F << 0);
}

void cfgIntExt(void)
{
	LPC_PINCON->PINSEL4 |= (0b010101 << 20); //P2.10, P2.11 y P2.11 como EINTX
	LPC_PINCON->PINMODE4 |= (0b11 << 20); //P2.10 con pull-down interna

	LPC_SC->EXTMODE |= (1<<2); // EINT2 por flanco
	LPC_SC->EXPOLAR |= (1<<0); // EINT0 activa por alto

	NVIC_EnableIRQ(EINT0_IRQn);
	NVIC_EnableIRQ(EINT1_IRQn);
	NVIC_EnableIRQ(EINT2_IRQn);
}

void cfgSysTick(void)
{
	SysTick->LOAD = 15999; // nTicks = (16e6)*(1e-3) - 1
	SysTick->VAL = 0;
	SysTick->CTRL = (7<<0);
}

void EINT0_IRQHandler(void)
{
	number = 0;
	while(LPC_GPIO2->FIOPIN & (1<<10)){
		LPC_GPIO0->FIOCLR &= ~(0x7F);
		LPC_GPIO0->FIOSET |= code[number]; // Muestro a la salida el numero 0
	}
	LPC_SC->EXTINT |= (1<<0);
}

void EINT1_IRQHandler(void)
{
	status = !status; // status == 1, significa que la cuenta debe pararse
	LPC_SC->EXTINT |= (1<<1);
}

void EINT2_IRQHandler(void)
{
	static uint32_t vTicks = 0;
	vTicks++;
	if(vTicks % 2){
		overflows_goal = 0;
	}else{
		overflows_goal = 1000;
	}
	LPC_SC->EXTINT |= (1<<2);
}

void SysTick_Handler(void){
	static uint32_t overflows = 0;
	if(!status){
		if(overflows_goal == 0){
			number = (number + 1) % 10;
			LPC_GPIO0->FIOCLR &= ~(0x7F);
			LPC_GPIO0->FIOSET |= code[number];
		}else{
			overflows++;
			if(overflows == overflows_goal){
				number = (number + 1) % 10;
				LPC_GPIO0->FIOCLR &= ~(0x7F);
				LPC_GPIO0->FIOSET |= code[number];
			}
		}
	}
	SysTick->CTRL &= SysTick->CTRL;
}
