#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_adc.h"

#include <stdio.h>

static uint8_t warning = 0;
volatile float last_measure;


void cfgGPIO(void);
void cfgADC(void);
void cfgTimer(void);

int main(void){

	cfgGPIO();
	cfgADC();
	cfgTimer();

	while(1){
		if(warning >= 10){
			GPIO_SetValue(0, 22); // Turn on Red Led 0.22
			GPIO_ClearValue(3, 25);
			GPIO_ClearValue(3, 26);
		}else if(last_measure < 40.0f){
			GPIO_SetValue(3, 25);// Turn on Green Led 3.25
			GPIO_ClearValue(0, 22);
			GPIO_ClearValue(3, 26);
		}else{
			GPIO_SetValue(3, 26);// Turn on Yellow Led 3.26
			GPIO_ClearValue(0, 22);
			GPIO_ClearValue(3, 25);
		}
	}
}

void cfgGPIO(void){
	PINSEL_CFG_Type cfgGreenLed;
	PINSEL_CFG_Type cfgYellowLed;
	PINSEL_CFG_Type cfgRedLed;
	PINSEL_CFG_TYpe cfgPinMAT0_CH1;

	cfgGreenLed.Portnum = PINSEL_PORT_3;
	cfgGreenLed.Pinnum = PINSEL_PIN_25;
	cfgGreenLed.Funcnum = PINSEL_FUNC_0;
	cfgGreenLed.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgGreenLed.OpenDrain = PINSEL_PINMODE_NORMAL;

	cfgYellowLed.Portnum = PINSEL_PORT_3;
	cfgYellowLed.Pinnum = PINSEL_PIN_26;
	cfgYellowLed.Funcnum = PINSEL_FUNC_0;
	cfgYellowLed.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgYellowLed.OpenDrain = PINSEL_PINMODE_NORMAL;

	cfgRedLed.Portnum = PINSEL_PORT_0;
	cfgRedLed.Pinnum = PINSEL_PIN_22;
	cfgRedLed.Funcnum = PINSEL_FUNC_0;
	cfgRedLed.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgRedLed.OpenDrain = PINSEL_PINMODE_NORMAL;


	cfgPinADC0_CH0.Portnum = PINSEL_PORT_0;
	cfgPinADC0_CH0.Pinnum = PINSEL_PIN_23;
	cfgPinADC0_CH0.Funcnum = PINSEL_FUNC_1;
	cfgPinADC0_CH0.OpenDrain = PINSEL_PINMODE_NORMAL;

	PINSEL_ConfigPin(&cfgGreenLed)
	PINSEL_ConfigPin(&cfgYellowLed)
	PINSEL_ConfigPin(&cfgRedLed)
	PINSEL_ConfigPin(&cfgPinMAT0_CH1)

	return;
}

void cfgTimer(void){

	TIM_TIMERCFG_Type cfgTimerMode;
	TIM_MATCHCFG_Type cfgTimerMatch;

	cfgTimerMode.PrescaleOption = TIM_PRESCALE_USVAL;
	cfgTimerMode.PrescaleValue = 1000;

	cfgTimerMatch.MatchChannel = 1;
	cfgTimerMatch.IntOnMatch = DISABLE;
	cfgTimerMatch.ResetOnMatch = ENABLE;
	cfgTimerMatch.StopOnMatch = DISABLE;
	cfgTimerMatch.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
	cfgTimerMatch.MatchValue = 49;

	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &cfgTimerMode);
	TIM_ConfigMatch(LPC_TIM0, &cfgTimerMatch);
	TIM_Cmd(LPC_TIM0, ENABLE);

	return;
}

void cfgADC(void){

	ADC_Init(LPC_ADC, 100);
	ADC_BurstCmd(LPC_ADC, DISABLE);
	ADC_StartCmd(LPC_ADC, ADC_START_ON_MAT01);
	ADC_EdgeStartConfig(LPC_ADC, ADC_START_ON_FALLING);
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE);
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN0, ENABLE);

	NVIC_EnableIRQ(ADC_IRQn);

	return;
}

void ADC_IRQHandler(void){
	if(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_0, ADC_DATA_DONE)){
		last_measure = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0)*100.0f/4095.0f;
		if(last_measure > 60.0f){
			warning++;
		}else{
			warning = 0;
		}
	}
}
