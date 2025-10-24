#ifdef __USE_CMSIS
#include "LPC17xx.h"
#include "lpc17xx_timer.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_dac.h"
#include "lpc17xx_gpdma.h"

#include <stdio.h>

#define ADC_BUFFER_START 0x2007C000
#define BUFFER_SIZE 16

volatile uint32_t *adc_samples0 = (volatile uint32_t *) ADC_BUFFER_START
volatile uint32_t *adc_samples1 = (volatile uint32_t *) (ADC_BUFFER_START + 16)

volatile uint32_t average = 0;

void cfgGPIO(void);
void cfgADC(void);
void cfgDAC(void);
void cfgDMA(void);

int main(void){

	cfgGPIO();
	cfgDAC()
	cfgDMA();
	cfgADC();

	while(1){

	};

	return 0;
}

void cfgGPIO(void){
	PINSEL_CFG_Type cfgPinADC0_CH3;
	cfgPinADC0_CH3.Portnum = PINSEL_PORT_0;
	cfgPinADC0_CH3.Pinnum = PINSEL_PORT_25;
	cfgPinADC0_CH3.Funcnum = PINSEL_FUNC_1;
	cfgPinADC0_CH3.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinADC0_CH3.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&cfgPinADC0_CH3);

	PINSEL_CFG_Type cfgPinDAC;
	cfgPinDAC.Portnum = PINSEL_PORT_0;
	cfgPinDAC.Pinnum = PINSEL_PORT_26;
	cfgPinDAC.Funcnum = PINSEL_FUNC_2;
	cfgPinDAC.Pinmode = PINSEL_PINMODE_TRISTATE;
	cfgPinDAC.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&cfgPinDAC);

	return;
}

void cfgADC(void){
	ADC_Init(LPC_ADC, 60000);
	ADC_BurstCmd(LPC_ADC, ENABLE);
	//ADC_StartCmd(LPC_ADC, ADC_START_CONTINUOUS);
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_3, ENABLE);
	ADC_IntConfig(LPC_ADC, ADC_ADINTEN3, ENABLE);

	NVIC_EnableIRQ(ADC_IRQn);

	return;
}

void cfgDAC(void){
	DAC_Init(LPC_DAC);
}

void cfgDMA(void){
	GPDMA_LLI_Type cfgADC_LLI0_CH0;
	GPDMA_Channel_CFG_Type cfgADC_DMA_CH0;

	NVIC_DisableIRQ(DMA_IRQn);
	GPDMA_Init();

	cfgADC_LLI0_CH0.SrcAddr = (uint32_t)&(LPC_ADC->ADDR0);
	cfgADC_LLI0_CH0.DestAddr = (uint32_t)adc_samples0;
	cfgADC_LLI0_CH0.NextLLI = (uint32_t)&cfgADC_LLI1_CH0;
	cfgADC_LLI0_CH0.Control = (ADC_BUFFER_SIZE<<0)
							|= (2<<18)
							|= (2<<21)
							& ~(1<<26)
							|= (1<<27);

	cfgADC_LLI1_CH0.SrcAddr = (uint32_t)&(LPC_ADC->ADDR0);
	cfgADC_LLI1_CH0.DestAddr = (uint32_t)adc_samples1;
	cfgADC_LLI1_CH0.NextLLI = (uint32_t)&cfgADC_LLI0_CH0;
	cfgADC_LLI1_CH0.Control = (ADC_BUFFER_SIZE<<0)
							|= (2<<18)
							|= (2<<21)
							& ~(1<<26)
							|= (1<<27);

	cfgADC_DMA_CH0.ChannelNum = 0;
	cfgADC_DMA_CH0.TransferSize = ADC_BUFFER_SIZE;
	cfgADC_DMA_CH0.TransferWidth = 0;
	cfgADC_DMA_CH0.SrcMemAddr = 0;
	cfgADC_DMA_CH0.DestMemAddr = (uint32_t)adc_samples0;
	cfgADC_DMA_CH0.SrcConn = GPDMA_CONN_ADC;
	cfgADC_DMA_CH0.DestConn = 0;
	cfgADC_DMA_CH0.DMALLI = (uint32_t)&cfgADC_LLI0_CH0;
	cfgADC_DMA_CH0.TransferType = GPDMA_TRANSFERTYPE_P2M;

	GPDMA_ChannelCmd(LPC_GPDMACH0, ENABLE);
	GPDMA_Setup(&cfgADC_DMA_CH0);
}

void DMA_IRQHandler(void){
	static uint32_t list = 0;
	if(GPDMA_IntGetStatus(GPDMA_STAT_INTTC, 0)){
		list++;
		if(list%2){
			for(uint8_t inte = 0; inte < ADC_BUFFER_SIZE; inte++){
				average += (uint32_t) (*(adc_samples1 + inte) >> 4);
			}
			average = average/ADC_BUFFER_SIZE;
		}else{
			for(uint8_t inte = 0; inte < ADC_BUFFER_SIZE; inte++){
				average += (uint32_t) (*(adc_samples1 + inte) >> 4);
			}
			average = average/ADC_BUFFER_SIZE;
		}
		DAC_UpdateValue(LPC_DAC, average);
		average = 0;
	}
}
