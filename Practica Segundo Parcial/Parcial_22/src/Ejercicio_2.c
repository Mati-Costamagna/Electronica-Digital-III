/*
 * Utilizando el timer0, un dac, interrupciones y el driver del LPC1769, escribir un codigo que permita
 * generar una senal triangular periodica simetrica, que tenga el minimo periodo posible, la maxima
 * excursion de voltaje pico a pico posible y el minimo incremento de senal posible por el dac. Suponer
 * una frecuencia de cclk de 100 MHz.
 *
 * 	Created on: Sep 9, 2025
 *  Author: Matias Costamagna
 *
 */


#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_gpdma.h"

#define NUM_WAVE_SAMPLES (1024*2 - 1)
static uint32_t waveform[NUM_WAVE_SAMPLES];

void cfgPBC(void);
void cfgTimer0(void);
void cfgDAC(void);
void generarWaveform(void);

int main(void)
{
	generarWaveform();
	cfgPBC();
	cfgTimer0();
	cfgDAC();

	while(1){};

	return 0;
}

void cfgPBC(void){
	PINSEL_CFG_Type pinDAC;
	pinDAC.Portnum = PINSEL_PORT_0;
	pinDAC.Pinnum = PINSEL_PIN_26;
	pinDAC.Funcnum = PINSEL_FUNC_2;
	pinDAC.Pinmode = PINSEL_PINMODE_TRISTATE;
	pinDAC.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&pinDAC);
}

void generarWaveform(void){
	for(uint32_t inte = 0; inte < 1024; inte ++){
		waveform[inte] = inte;
	}
	for(uint32_t inte = 1022; inte < 0; inte --){
		waveform[1024 + (1023 - inte)] = inte;
	}
}

void cfgTimer(void){
	TIM_TIMERCFG_Type timerMode;
	timerMode.PrescaleOption = TIM_PRESCALE_USVAL;
	timerMode.PrescaleValue = 0;

	TIM_MATCHCFG_Type timerMatch;
	timerMatch.MatchChannel = 0;
	timerMatch.IntOnMatch = ENABLE;
	timerMatch.StopOnMatch = DISABLE;
	timerMatch.ResetOnMatch = ENABLE;
	timerMatch.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	timerMatch.MatchValue = 1;

	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timerMode);
	TIM_ConfigMatch(LPC_TIM0, &timerMatch);
	TIM_Cmd(LPC_TIM0, ENABLE);

	NVIC_EnableIRQ(TIMER0_IRQn);
}

void cfgDAC(void){
	DAC_Init(LPC_DAC);
}

void TIMER0_IRQHandler(void){
	static uint32_t indexWave = 0;
	DAC_UpdateValue(LPC_DAC, waveform[indexWave]);
	indexWave = (indexWave + 1) % NUM_WAVE_SAMPLES;
	TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
}
