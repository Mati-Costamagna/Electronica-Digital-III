/*
 * 1.- Configure el Systick Timer de modo que genere una forma de onda
 * llamada PWM. Esta señal debe ser extraida por el pin P0.22 para que
 * controle la intensidad de brillo del led. El periodo de la señal debe ser
 * de 10ms con un duty cycle de 10%. Configure la interrupción externa
 * EINT0 de modo que cada vez que se entre en una rutina de
 * interrupción externa el duty cycle incremente en un 10% (1ms). Esto se
 * repite hasta llegar al 100%, luego, si se entra nuevamente a la
 * interrupción externa, el duty cycle volverá al 10%.
 * 2.- Modificar los niveles de prioridad para que la interrupción por systick
 * tenga mayor prioridad que la interrupción externa.
 *
 * 	Created on: Sep 2, 2025
 *  Author: Matias Costamagna
 *
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif


#include <stdio.h>


int main(void) {

    return 0 ;
}
