/*
 * Un estacionamiento automatizado utiliza una barrera que se abre y cierra en funcion de la
 * validacion de un ticket de acceso utilizando un LPC1769 Rev. D trabajando a una frecuencia
 * de CCLK a 70[MHz].
 *
 * Cuando el sistema detecta que un automovil se ha posicionado frente a la barrera, se debe
 * activar un sensor conectado al pin P2[4] mediante una interrupcion externa (EINT). Una vez
 * validado el ticket, el sistema activa un motor que abre la barrera usando el pin P0[15]. El
 * motor debe estar activado por X segundos y luego apagarse, utilizando el temporizador
 * SysTick para contar el timepo. Si el ticket es invalido, se encendera un LED rojo conectado
 * al pin P1[5].
 *
 * Para gestionar el timepo de apertura de la abrra, existe un switch conectado al pin P3[4]
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
 * 			0x03								|		40 seg
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

#define SYST_200MS_LOAD  (13999999) // 70e6 * 0.2 - 1

#define OVR_3S   15
#define OVR_5S   25
#define OVR_10S  50
#define OVR_20S 100
#define OVR_40S 200

typedef enum { IDLE, CONFIG, OPENING } state_t;
volatile state_t state = IDLE;
volatile uint32_t overflow_count = 0;
volatile uint32_t overflow_goal = 0;
volatile uint8_t switch_count = 0;

void cfgGPIO(void);
void cfgIntExt(void);
void systickStart(void);
void systickStop(void);
uint32_t switchCounter(void);
bool validarTicket(void)


int main(void)
{

	cfgGPIO();

	cfgIntExt();

	while(1){};

    return 0 ;
}

void cfgGPIO(void)
{
	LPC_PINCON->PINSEL0 &= ~(3 << 30);
	LPC_GPIO0->FIODIR |= (1 << 15);
	LPC_GPIO0->FIOCLR |= (1 << 15);

	LPC_PINCON->PINSEL2 &= ~(3 << 10);
	LPC_GPIO1->FIODIR |= (1 << 5);
	LPC_GPIO1->FIOCLR |= (1 << 5);

	LPC_PINCON->PINSEL6 &= ~(3 << 8);
	LPC_PINCON->PINMODE6 &= ~(3 << 8);
	LPC_GPIO3->FIODIR &= ~(1 << 4);

}

void cfgIntExt(void)
{
	LPC_PINCON->PINSEL4 |= (0b01 << 20);
	LPC_SC->EXTMODE |= (1 << 0);
	LPC_SC->EXTPOLAR |= (1 << 0);
	LPC_SC->EXTINT |= (1 << 0);
	NVIC_EnableIRQ(EINT0_IRQn);
}

void systickStart(void)
{
	SysTick->LOAD = nTicks_200ms;
	SysTick->VAL = 0;
	SysTick->CTRL = (7 << 0);
}

void systickStop(void)
{
	SysTick->CTRL = 0;
}


void ticketValido(void)
{
	SysTick->LOAD = nTicks_200ms;
	SysTick->VAL = 0;
	SysTick->CTRL = (1 << 0);
	while(overflow_goal == nOverflow_3s){
		if(LPC_GPIO3->FIOPIN & (1<<4)){
			switch_count++;
		}
	}
}

uint32_t switchCounter(uint8_t switch_count)
{
	switch(switch_count){          // 0..3, y 4 → 0 por tabla
		case 0:  return OVR_5S;
		case 1:  return OVR_10S;
		case 2:  return OVR_20S;
		case 3:  return OVR_40S;
		case 4:  return OVR_5s;
		default: return OVR_5S;
	}
}


void EINT0_IRQHandler(void)
{
	switch_count = 0;
	if(validarTicket()){
		state = CONFIG;
		overflow_count = 0;
		switch_count = 0;
		systickStart();
	}else{
		LPC_GPIO1->FIOSET |= (1<<5);
	}
	LPC_SC->EXTINT |= (1<<0);
}

void SysTick_Handler(void){
    switch(state){
    case CONFIG: {
    	overflow_count++;
        // Lee flanco del switch (P0.4), muy simple (sin debounce)
        static uint8_t last = 0;
        uint8_t now = (LPC_GPIO0->FIOPIN >> 4) & 1u;
        if(now && !last) switch_count++;
        last = now;

        if(overflow_count >= OVR_3S){
        	overflow_goal = switchCounter(switch_count);
            overflow_count = 0;
            LPC_GPIO0->FIOSET = (1<<15); // motor ON
            state = OPENING;
        }
        break;
    }
    case OPENING:
    	overflow_count++;
        if(overflow_count >= overflow_goal){
            LPC_GPIO0->FIOCLR = (1<<15); // motor OFF
            systickStop();
            state = IDLE;
        }
        break;
    default:
        break;
    }
}

bool validarTicket(void)
{
	return true;
}
