/*
 * Escribir un programa en C que permita realizar un promedio móvil con los
 * últimos 8 datos ingresados por el puerto 1. Considerar que cada dato es un
 * entero signado y está formado por los 16 bits menos significativos de dicho
 * puerto. El resultado, también de 16 bits, debe ser sacado por los pines P0.0 al
 * P0.11 y P0.15 al P0.18. Recordar que en un promedio móvil primero se
 * descarta el dato mas viejo de los 8 datos guardados, se ingresa un nuevo dato
 * proveniente del puerto y se realiza la nueva operación de promedio con esos 8
 * datos disponibles, así sucesivamente. Considerar el uso de un retardo antes de
 * tomar una nueva muestra por el puerto.
 *
 * 	Created on: Aug 29, 2025
 *  Author: Matias Costamagna
 *
 */

#ifdef _USE_CMSIS
#include "LPC17xx.h"
#endif

#include <stdio.h>

#define OUTPUT_pins (0xF0FFF << 0)
#define INPUT_pins (0xFFFF << 0)
#define DATA_SIZE 8

void cfgGPIO(void);
void add_value(int32_t data[], int32_t value, uint8_t *idx);
int32_t average(int32_t data[]);
void show_average(int32_t avrg);
void delay(uint32_t delay_time);

int main(void)
{
	int32_t datos[DATA_SIZE] = {0};
	int32_t avrg;
	uint8_t idx =0;
	uint32_t delay_time = 10000;

	cfgGPIO();

	while(1){
		int32_t value = (LPC_GPIO1 -> FIOPIN & INPUT_pins);

		add_value(datos, value, &idx);

		avrg = average(datos);

		show_average(avrg);

		delay(delay_time);
	}
	return 0;
}

void cfgGPIO(void)
{
	LPC_GPIO1 -> FIODIR &= ~INPUT_pins; //P1.0 to P.15 as input

	LPC_GPIO0 -> FIODIR |= OUTPUT_pins; //P0.0 to P.11 and P.015 to P.18 as output
	LPC_GPIO0 -> FIOCLR = OUTPUT_pins; //Cleaning output pins
}

void add_value(int32_t data[], int32_t value, uint8_t *idx)
{
	data[*idx] = value;
	*idx = (*idx + 1) % DATA_SIZE;
}

int32_t average(int32_t data[])
{
	uint32_t counter;
	int32_t sum = 0;

	for(counter = 0; counter < DATA_SIZE; counter++){
		sum += data[counter];
	}
	return sum /= DATA_SIZE;
}

void show_average(int32_t avrg)
{
	int32_t refactored_avrg = (((avrg & 0xF000) << 3) | (avrg & 0x0FFF));
	LPC_GPIO0 -> FIOCLR = OUTPUT_pins;
	LPC_GPIO0 -> FIOSET = refactored_avrg;
}

void delay(uint32_t delay_time)
{
	uint32_t counter;
	for(counter = 0; counter < delay_time; counter ++) {};
}
