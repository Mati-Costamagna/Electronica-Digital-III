/*
 * Programar el microcontrolador LPC1769 en un código de lenguaje C para que mediante su ADC
 * digitalice una señal analógica cuyo ancho de banda es de 16 khz. La señal analógica tiene una
 * amplitud de pico máxima positiva de 3.3 voltios. Los datos deben ser guardados utilizando el
 * Hardware GDMA en la primera mitad de la memoria SRAM ubicada en el bloque AHB SRAM -
 * bank 0, de manera tal que permita almacenar todos los datos posibles que esta memoria nos
 * permita. Los datos deben ser almacenados como un buffer circular conservando siempre las últimas
 * muestras.
 * Por otro lado se tiene una forma de onda como se muestra en la imagen (Forma de rayo). Esta señal
 * debe ser generada por una función y debe ser reproducida por el DAC desde la segunda mitad de
 * AHB SRAM - bank 0 memoria utilizando DMA de tal forma que se logre un periodo de 614us
 * logrando la máxima resolución y máximo rango de tensión.
 * Durante operación normal se debe generar por el DAC la forma de onda mencionada como
 * wave_form. Se debe indicar cuál es el mínimo incremento de tensión de salida de esa forma de onda.
 * Cuando interrumpe una extint conectada a un pin, el ADC configurado debe completar el ciclo de
 * conversión que estaba realizando, y ser detenido, a continuación se comienzan a sacar las muestras del ADC por el DAC utilizando DMA y desde las posiciones de memoria originales.
 * Cuando interrumpe nuevamente en el mismo pin, se vuelve a repetir la señal del DAC generada por la forma de onda de wave_form previamente almacenada y se arranca de nuevo la conversión de datos del adc. Se alterna así entre los dos estados del sistema con cada interrupción externa.
 * Suponer una frecuencia de core cclk de 100 Mhz. El código debe estar debidamente comentado.
 *
 * 	Created on: Oct 24, 2025
 *  Author: Matias Costamagna
 *
 */


#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_eint.h"

#define WAVE_SAMPLES (614)
#define WAVEFORM_START ()
#define SAMPLES_START ()
#define ADC_SAMPLES ((WAVEFORM_START - SAMPLES_START)/sizeof(uint32_t))

volatile uin32_t  *wave_form = (volatile uint32_t *)WAVEFORM_START;

void cfgPBC(void);
void cfgADC(void);
void cfgDAC(void);
void cfgDMA(void);
void cfgEINT(void);
void generarWaveform(void);

int main(void){

	generarWaveform();
	cfgPBC();
	cfgADC();
	cfgDAC();
	cfgDMA();
	cfgEINT();

	while(1){};
	
	return 0;
}

void generarWaveform(void){
	for(int32_t inte = 0; inte < WAVE_SAMPLES/2; inte++){
		wave_form[inte] = (uint32_t) (inte * 1.67f + 512);
	}
	for(int32_t inte = 0; inte <= WAVE_SAMPLES/2; inte++){
		wave_form[inte + WAVE_SAMPLES/2] = (uint32_t) (inte*1.67f);
	}
}

void cfgPBC(void){
	PINSEL_CFG_Type pinADC;
	pinADC.Portnum = PINSEL_PORT_0;
	pinADC.Pinnnum = PINSEL_PIN_23;
	pinADC.Funcnum = PINSEL_FUNC_1;
	pinADC.Pinmode = PINSEL_PINMODE_TRISTATE;
	pinADC.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&pinADC);

	PINSEL_CFG_Type pinDAC;
	pinDAC.Portnum = PINSEL_PORT_0;
	pinDAC.Pinnnum = PINSEL_PIN_26;
	pinDAC.Funcnum = PINSEL_FUNC_2;
	pinDAC.Pinmode = PINSEL_PINMODE_TRISTATE;
	pinDAC.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&pinDAC);

	PINSEL_CFG_Type pinEINT;
	pinEINT.Portnum = PINSEL_PORT_2;
	pinEINT.Pinnnum = PINSEL_PIN_10;
	pinEINT.Funcnum = PINSEL_FUNC_1;
	pinEINT.Pinmode = PINSEL_PINMODE_TRISTATE;
	pinEINT.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&pinEINT);
}

void cfgADC(void){
	ADC_Init(LPC_ADC, 32000);
	ADC_BurstCmd(LPC_ADC, ENABLE);
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0);
}

void cfgDAC(void){
	DAC_Init(LPC_DAC);
}

void cfgTimer(void){
	TIM_TIMER_CFG timerMode;
	timerMode.PrescaleOption = TIM_PRESCALE_USVAL;
	timerMode.PrescaleValue = 100;

	TIM_MATCHCFG_Type timerMatch;
	timerMatch.MatchChannel = 0;
	timerMatch.IntOnMatch = ENABLE;
	timerMatch.StopOnMatch = DISABLE;
	timerMatch.ResetOnMatch = ENABLE;
	timerMatch.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
	timerMatch.MatchValue = 6;

	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timerMode);
	TIM_ConfigMatch(LPC_TIM0, &timerMatch);
	TIM_Cmd(LPC_TIM0, ENABLE);

	NVIC_EnableIRQ(TIMER0_IRQn);
}

void cfgEINT(void){
	EXTI_InitTyoeDef cfgEINT0;
	cfgEINT0.EXTI_Line = EXTI_EINT0;
	cfgEINT0.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
	cfgEINT0.EXTI_polarity = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;
	EXTI_Config(&cfgEINT0);

	NVIC_EnableIRQ(EINT0_IRQn);
}

void cfgDMA(void){
	GPDMA_LLI_Type cfgADC_LLI0_CH0;
	GPDMA_Channel_CFG_Type cfgADC_DMA_CH0;
	GPDMA_LLI_Type cfgDAC_LLI0_CH1;
	GPDMA_Channel_CFG_Type cfgDAC_DMA_CH1;
	GPDMA_LLI_Type cfgADC_DAC_DMA_LLI;
	GPDMA_Channel_CFG_Type cfgADC_DAC_DMA;
	
	NVIC_DisableIRQ(DMA_IRQn);
	GPDMA_Init();

	cfgADC_LLI0_CH0.SrcAddr = (uint32_t)&(LPC_ADC->ADDR0);
	cfgADC_LLI0_CH0.DestAddr = (uint32_t)SAMPLES_START;
	cfgADC_LLI0_CH0.NextLLI = (uint32_t)&cfgADC_LLI0_CH0;
	cfgADC_LLI0_CH0.Control = (ADC_SAMPLES<<0)|(2<<18)|(2<21)&~(1<<26)|(1<<27);

	cfgADC_DMA_CH0.ChannelNum = 0;
	cfgADC_DMA_CH0.TransferSize = ADC_SAMPLES;
	cfgADC_DMA_CH0.SrcMemAddr = 0;
	cfgADC_DMA_CH0.DestMemAddr = (uint32_t)SAMPLES_START;
	cfgADC_DMA_CH0.TransferType = GPDMA_TRANSFERTYPE_P2M;
	cfgADC_DMA_CH0.SrcConn = GPDMA_CONN_ADC;
	cfgADC_DMA_CH0.DestConn = 0;
	cfgADC_DMA_CH0.DMALLI = &cfgADC_LLI0_CH0;

	cfgADC_LLI0_CH1.SrcAddr = (uint32_t)WAVEFORM_START;
	cfgADC_LLI0_CH1.DestAddr = (uint32_t)&(LPC_DAC->DACR);
	cfgADC_LLI0_CH0.NextLLI = (uint32_t)&cfgADC_LLI0_CH1;
	cfgADC_LLI0_CH1.Control = (WAVE_SAMPLES<<0)|(2<<18)|(2<21)&~(1<<27)|(1<<26);

	cfgDAC_DMA_CH1.ChannelNum = 1;
	cfgDAC_DMA_CH1.TransferSize = WAVE_SAMPLES;
	cfgDAC_DMA_CH1.SrcMemAddr = (uint32_t)WAVEFORM_START;
	cfgDAC_DMA_CH1.DestMemAddr = 0;
	cfgDAC_DMA_CH1.TransferType = GPDMA_TRANSFERTYPE_M2P;
	cfgDAC_DMA_CH1.SrcConn = 0;
	cfgDAC_DMA_CH1.DestConn = GPDMA_CONN_DAC;
	cfgDAC_DMA_CH1.DMALLI = &cfgADC_LLI0_CH1;

	cfgADC_DAC_DMA_LLI.SrcAddr = (uint32_t)SAMPLES_START;
	cfgADC_DAC_DMA_LLI.DestAddr = (uint32_t)&(LPC_DAC->DACR);
	cfgADC_DAC_DMA_LLI.NextLLI = (uint32_t)&cfgADC_DAC_DMA_LLI;
	cfgADC_DAC_DMA_LLI.Control = (ADC_SAMPLES<<0)|(2<<18)|(2<21)&~(1<<27)|(1<<26);

	cfgADC_DAC_DMA.ChannelNum = 2;
	cfgADC_DAC_DMA.TransferSize = ADC_SAMPLES;
	cfgADC_DAC_DMA.SrcMemAddr = (uint32_t)SAMPLES_START;
	cfgADC_DAC_DMA.DestMemAddr = 0;
	cfgADC_DAC_DMA.TransferType = GPDMA_TRANSFERTYPE_M20;
	cfgADC_DAC_DMA.SrcConn = 0;
	cfgADC_DAC_DMA.DestConn = GPDMA_CONN_DAC;
	cfgADC_DAC_DMA.DMALLI = &cfgADC_DAC_DMA_LLI;

	GPDMA_Setup(&cfgADC_DMA_CH0);
	GPDMA_Setup(&cfgDAC_DMA_CH1);
	GPDMA_Setup(&cfgADC_DAC_DMA);
}

void EINT0_IRQHandler(void){
	static uint8_t dac_status = 0;
	while(!ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_0, ADC_DATA_DONE)){};
	if(dac_status){
		ADC_DeInit(LPC_ADC);
		GPDMA_ChannelCmd(0, DISABLE);
		GPDMA_ChannelCmd(1, DISABLE);
		GPDMA_ChannelCmd(2, ENABLE);
	}else{
		ADC_Init(LPC_ADC, 32000);
		GPDMA_ChannelCmd(0, ENABLE);
		GPDMA_ChannelCmd(1, ENABLE);
		GPDMA_ChannelCmd(2, DISABLE);
	}
	dac_status = (dac_status+1)%2;
	
	EXTI_ClearEXTIFlag(EXTI_EINT0);
}