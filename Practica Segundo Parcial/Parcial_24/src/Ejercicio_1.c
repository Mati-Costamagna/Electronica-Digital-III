/*
 * Por un pin del ADC del microcrontrolador LPC1769 Rev D, ingresa una tension de rango dinamico
 * -2[V] a 2[V] con una frecuencia maxima de 20 [kHz], obtenidos de un sensor de presion diferencial
 * que mide las variaciones de voltaje en funcion de la diferencia de presion a lo largo de una tuberia
 * de GNL. Se pide almacenar la señal, cumpliendo criterio de Nyquist, en la posicion de memoria 0x2008E000
 * y con un tamaño de 1kB, una vez ocupado todo el espacio, se vuelve a almacenar desde el comienzo.
 *
 * En funcion del promedio de todas las muestras obtenidas cada 100[ms] de la señal capturada, se debe
 * tomar una decision sobre dos pines de salida GPIO que generan señales cuadradas de 3.3[V] en fase (Señales 
 * S1 y S2, usadas para ajustar dos valvulas de control de flujo que regulan la presion del gas en diferentes
 * secciones de la planta) y con una frecuencia de 10[kHz]:
 * 1. Si el promedio se encuentra en el rango de -2[V] < Promedio < 0[V], la senal de control de la valvula S1
 * se adelanta resepecto a la de S2 en un angulo proporcional al valor del promedio (rango de 0 a 180 grados).
 * 2. Si el promedio se encuentra en el rango de 0[V] < Promedio < 2[V], la senal de control de la valvula S2
 * se adelanta resepecto a la de S1 en un angulo proporcional al valor del promedio (rango de 0 a 180 grados).
 *
 * 	Created on: Oct 20, 2025
 *  Author: Matias Costamagna
 *
 */

#ifdef __USE_CMSIS
#include "LPC17xx.h"
#endif

#include <cr_section_macros.h>

#include <stdio.h>

#define BUFFER_START 0x2008E000
#define BUFFER_SIZE 1024
#define WAVEFORM_FREQ 10000

volatile uint32_t *adc_samples = (volatile uint32_t *) BUFFER_START;
volatile uint32_t average = 0;

void cfgADC();
void cfgGPIO();
void cfgTimer0();
void cfgDMA();
void waveformGeneration(uint32_t avrg);

int main(void)
{

	while(1){};

    return 0 ;
}

void cfgGPIO(){
	PINSEL_CFG_Type PinCfgADC;
	PinCfgADC.Portnum = PINSEL_PORT_X;
	PinCfgADC.Pinnum = PINSEL_PIN_X;
	PinCfgADC.Funcnum = PINSEL_FUNC_1;
	PinCfgADC.Pinmode = PINSEL_PINMODE_TRISTATE;
	PinCfgADC.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&PinCfgADC);

	PINSEL_CFG_Type PinCfgTim0;
	PinCfgTim0.Portnum = PINSEL_PORT_W;
	PinCfgTim0.Pinnum = PINSEL_PIN_W;
	PinCfgTim0.Funcnum = PINSEL_FUNC_1;
	PinCfgTim0.Pinmode = PINSEL_PINMODE_TRISTATE;
	PinCfgTim0.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&PinCfgTim0);

	PINSEL_CFG_Type PinCfgS1;
	PinCfgS1.Portnum = PINSEL_PORT_Y;
	PinCfgS1.Pinnum = PINSEL_PIN_Y;
	PinCfgS1.Funcnum = PINSEL_FUNC_0;
	PinCfgS1.Pinmode = PINSEL_PINMODE_TRISTATE;
	PinCfgS1.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&PinCfgS1);
	GPIO_SetDir(PINSEL_PORT_Y, (1<<PINSEL_PIN_Y), 1); // Salida S1

	PINSEL_CFG_Type PinCfgS2;
	PinCfgS2.Portnum = PINSEL_PORT_Z;
	PinCfgS2.Pinnum = PINSEL_PIN_Z;
	PinCfgS2.Funcnum = PINSEL_FUNC_0;
	PinCfgS2.Pinmode = PINSEL_PINMODE_TRISTATE;
	PinCfgS2.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&PinCfgS2);
	GPIO_SetDir(PINSEL_PORT_Z, (1<<PINSEL_PIN_Z), 1); // Salida S2

	return;
}

void cfgADC(){
	ADC_Init(LPC_ADC, 40000);
	ADC_BursCmd(LPC_ADC, DISABLE);
	ADC_StartCmd(LPC_ADC, ADC_START_ON_MAT01);
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE);
	ADC_EdgeStartConfig(LPC_ADC, ADC_START_ON_FALLING);
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN0, DISABLE);
}

void cfgTimer0(){
	TIM_TIMERCFG_Type cfgTimerMode0;
	TIM_MATCHCFG_Type cfgTimerMatch;
	

	cfgTimerMode0.PrescaleOption = TIM_PRESCALE_USVAL;
	cfgTimerMode0.PrescaleValue = 1000;

	cfgTimerMatch.MatchChannel = 1;
	cfgTimerMatch.IntOnMatch = DISABLE;
	cfgTimerMatch.StopOnMatch = DISABLE;
	cfgTimerMatch.ResetOnMatch = ENABLE;
	cfgTimerMatch.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
	cfgTimerMatch.MatchValue = 49;
	TIM_ConfigMatch(LPC_TIM0, &cfgTimerMatch);

	cfgTimerMatch.MatchChannel = 1;
	cfgTimerMatch.IntOnMatch = DISABLE;
	cfgTimerMatch.StopOnMatch = DISABLE;
	cfgTimerMatch.ResetOnMatch = ENABLE;
	cfgTimerMatch.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
	cfgTimerMatch.MatchValue = 49;
	TIM_ConfigMatch(LPC_TIM0, &cfgTimerMatch);

	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &cfgTimerMode0);
	TIM_Cmd(LPC_TIM0, ENABLE);

	return;
}

void cfgDMA(){
	GPDMA_Channel_CFG_Type cfgADC_DMA_CH0;
	GPDMA_LLI_Type cfgADC_LLI0_CH0;

	NVIC_DisableIRQ(DMA_IRQn);
	GPDMA_Init();

	cfgADC_LLI0_CH0.SrcAddr = (uint32_t)&(LPC_ADC->ADDR0);
	cfgADC_LLI0_CH0.DestAddr = (uint32_t)adc_samples;
	cfgADC_LLI0_CH0.NextLLI = &cfgADC_LLI0_CH0;
	cfgADC_LLI0_CH0.Control = (BUFFER_SIZE<<0)
							|= (2<<18)
							|= (2<<21)
							& ~(1<<26)
							|= (1<<27);

	cfgADC_DMA_CH0.ChannelNum = 0;
	cfgADC_DMA_CH0.TransferSize = BUFFER_SIZE;
	cfgADC_DMA_CH0.SrcMemAddr = 0;
	cfgADC_DMA_CH0.DestMemAddr = (uint32_t)adc_samples;
	cfgADC_DMA_CH0.TransferType = GPDMA_TRANSFERTYPE_P2M;
	cfgADC_DMA_CH0.SrcConn = GPDMA_CONN_ADC;
	cfgADC_DMA_CH0.DestConn = 0;
	cfgADC_DMA_CH0.DMALLI = (uint32_t)&cfgADC_LLI0_CH0;

	GPDMA_ChannelCmd(LPC_GPDMACH0, ENABLE);
	GPDMA_Setup(&cfgADC_DMA_CH0);

	NVIC_EnableIRQ(DMA_IRQN);

	return;
}

void DMA_IRQHandler(){
	if(GPDMA_IntGetStatus(GPDMA_STAT_INTTC, 0)){
		average = 0;
		for(uint32_t inte = 0; inte < BUFFER_SIZE; inte++){
			average += *(adc_samples + inte);
		}
		average /= BUFFER_SIZE;
	}
}

void waveformGeneration(uint32_t avrg){
	phase_diff = (uint32_t) avrg * 180 / 1024;
	uint32_t time_offset = phase_diff / ((uint32_t)WAVEFORM_FREQ);

}