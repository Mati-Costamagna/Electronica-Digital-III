#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_gpdma.h"
#include "lpc17xx_exti.h"


#include <stdio.h>

#define ADC_START_BUFFER0 0x2007C000
#define ADC_START_BUFFER1 0x20080000
#define ADC_BUFFER_SIZE ((ADC_START_BUFFER1 - ADC_START_BUFFER0)/sizeof(uint32_t))

volatile uint32_t *adc_samples0 = (uint32_t *) ADC_START_BUFFER0
volatile uint32_t *adc_samples1 = (uint32_t *) ADC_START_BUFFER1
volatile boolean storeData_ptr_flag = false;

void cfgGPIO(void);
void cfgADC(void);
void cfgTimer(void);
void cfgDMA(void);
void cfgEXTI(void);

int main(void){

}

void cfgGPIO(void){
	PINSEL_CFG_Type pinExtInt;
	pinExtInt.Portnum = PINSEL_PORT_2;
	pinExtInt.Pinnum = PINSEL_PIN_10;
	pinExtInt.Funcnum = PINSEL_FUNC_1;
	pinExtInt.Pinmode = PINSEL_PINMODE_PULLDOWN;
	pinExtInt.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&pinExtInt);

	PINSEL_CFG_Type pinADC0_CH0;
	pinADC0_CH0.Portnum = PINSEL_PORT_0;
	pinADC0_CH0.Pinnum = PINSEL_PIN_23;
	pinADC0_CH0.Funcnum = PINSEL_FUNC_1;
	pinADC0_CH0.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&pinADC0_CH0);

	PINSEL_CFG_Type pinMAT0_CH1;
	pinMAT0_CH1.Portnum = PINSEL_PORT_1;
	pinMAT0_CH1.Pinnum = PINSEL_PIN_29;
	pinMAT0_CH1.Funcnum = PINSEL_FUNC_3;
	pinMAT0_CH1.Pinmode = PINSEL_PINMODE_TRISTATE;
	pinMAT0_CH1.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&pinMAT0_CH1);

	return;
}

void cfgADC(void){
	ADC_Init(LPC_ADC, 160000);
	ADC_BurstCmd(LPC_ADC, DISABLE);
	ADC_StartCmd(LPC_ADC, ADC_START_ON_MAT01);
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE);
	ADC_EdgeStartConfig(LPC_ADC, ADC_START_ON_FALLING);
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN0, ENABLE);

	NVIC_EnableIRQ(ADC_IRQn);
	return;
}

void cfgTimer(void){
	TIM_TIMERCFG_Type cfgTimerMode;
	cfgTimerMode.PrescaleOption = TIM_PRESCALE_USVAL;
	cfgTimerMode.PrescaleValue = 1000;

	TIM_MATCHCFG_Typer cfgTimerMatch;
	cfgTimerMatch.MatchChannel = 1;
	cfgTimerMatch.IntOnMatch = DISABLE;
	cfgTimerMatch.StopOnMatch = DISABLE;
	cfgTimerMatch.ResetOnMatch = ENABLE;
	cfgTimerMatch.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
	cfgTimerMatch.MatchValue = 499;

	TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &cfgTimerMode);
	TIM_ConfigMatch(LPC_TIM0, &cfgTimerMatch);
	TIM_Cmd(LPC_TIM0, ENABLE);

	return;
}

void cfgDMA(void){
	GPDMA_Channel_CFG_Type cfgADC_DMA_CH0;
	GPDMA_LLI_Type cfgADC_LLI0_CH0;

	NVIC_DisableIRQ(DMA_IRQn);
	GPDMA_Init();

	cfgADC_LLI0_CH0.SrcAddr = (uint32_t)&(LPC_ADC->ADDR0);
	cfgADC_LLI0_CH0.DestAddr = (uint32_t)adc_samples0
	cfgADC_LLI0_CH0.NextLLI = (uint32_t) &cfgADC_LLI0_CH0;
	cfgADC_LLI0_CH0.Control = (ADC_BUFFER_SIZE<<0)
						|= (2<<18)
						|= (2<<21)
						& ~(1<<26)
						|= (1<<27);

	cfgADC_DMA_CH0.ChannelNum = 0;
	cfgADC_DMA_CH0.TransferSize = ADC_BUFFER_SIZE;
	cfgADC_DMA_CH0.SrcMemAddr = 0;
	cfgADC_DMA_CH0.DestMemAddr = (uint32_t) &adc_samples0;
	cfgADC_DMA_CH0.TransferType = GPDMA_TRANSFERTYPE_P2M;
	cfgADC_DMA_CH0.SrcConn = GPDMA_CONN_ADC;
	cfgADC_DMA_CH0.DestConn = 0;
	cfgADC_DMA_CH0.DMALLI = &cfgADC_LLI0_CH0;

	GPDMA_Setup(&cfgADC_DMA_CH0);

	return;
}

void cfgEXTI(void){
	EXIT_InitTypeDef cfgEXTI0;
	cfgEXTI0.EXTI_Line = EXTI_EINT0;
	cfgEXTI0.EXTI_Mode = EXTI_MODE_EDGE_SENSITIVE;
	cfgEXTI0.EXTI_polarity = EXTI_POLARITY_LOW_ACTIVE_OR_FALLING_EDGE;
	EXTI_Config(&cfgEXTI0);

	NVIC_EnableIRQ(EINT0_IRQn);
	return;
}

void storeData_DMA(void){
	cfgGPDMA();
	storeData_ptr_flag = false;
}

void storeData_ptr(void){
	storeData_ptr_flag = true;
	GPDMA->DMACConfig & ~(1<<0);
}

void moveData(void){
	GPDMA_Channel_CFG_Type cfgMEM_DMA_CH7;

	NVIC_DisableIRQ(DMA_IRQn);
	GPDMA_Init();

	cfgMEM_DMA_CH7.ChannelNum = 7;
	cfgMEM_DMA_CH7.TransferSize = ADC_BUFFER_SIZE;
	cfgMEM_DMA_CH7.SrcMemAddr = (uint32_t) &adc_samples0;
	cfgMEM_DMA_CH7.DestMemAddr = (uint32_t) &adc_samples1;
	cfgMEM_DMA_CH7.TransferType = GPDMA_TRANSFERTYPE_M2M;
	cfgMEM_DMA_CH7.TransferWidth = GPDMA_WIDTH_WORD;
	cfgMEM_DMA_CH7.SrcConn = 0;
	cfgMEM_DMA_CH7.DestConn = 0;
	cfgMEM_DMA_CH7.DMALLI = 0;

	GPDMA_Setup(&cfgMEM_DMA_CH7);
}

void ADC_IRQHandler(void){
	static uint32_t ptr_iterator = 0;
	if(!(ADC_ChannelGetStatus(LPC_ADC, ADC_CHANNEL_0, ADC_DATA_DONE))){
		return;
	}else if(storeData_ptr_flag){
		uint32_t sample_value = (uint32_t) (ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0));
		*(adc_samples0 + ptr_iterator) = sample_value;
		ptr_iterator = (ptr_iterator + 1) % ADC_BUFFER_SIZE;
	}

	return;
}

void EINT0_IRQHandler(void){
	static uint32_t inte = 0;
	if(inte % 3 == 1){
		storeData_DMA();
	}else if (inte % 3 == 2){
		storeData_ptr();
	}else{
		moveData();
	}
	inte = (inte + 1)%3
}

void DMA_IRQHandler(void){
	if(GPDMA_IntGetStatus(GPDMA_STAT_INTTC, 7)){
		GPDMA_ClearIntPending(GPDMA_STATCLR_INTTC,7));
		GPDMA_ChannelCmd(7, DISABLE);
	}
}
