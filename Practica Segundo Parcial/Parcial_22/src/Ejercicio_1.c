/*
 * Programar el microcontrolador LPC1769 para que mediante su ADC digitalice dos senales analogicas
 * cuyos anchos de banda son de 10kHz cada una. Los canales utilizados deben ser el 2 y el 4 y los
 * datos deben ser guardados en dos regiones de memorias distintas que permitan contar con los ultimos
 * 20 datos de cada canal. Suponer una frecuencia de core cclk de 100Mhz.
 *
 * 	Created on: Oct 24, 2025
 *  Author: Matias Costamagna
 *
 */

#include "LPC17xx.h"
#include "lpc17xx_adc.h"

#define BUFFER_SIZE (20)
#define ADC_BUFFER_START0 (0x2007C000)
#define ADC_BUFFER_START1 (0x2007E000)

void cfgPBC(void);
void cfgADC(void);
void cfgDMA(void);


int main(void)
{
	cfgPBC();
	cfgDMA();
	cfgADC();

	while(1){};

    return 0 ;
}

void cfgPBC(void){
	PINSEL_CFG_Type pinADC_CH2;
	pinADC_CH2.Portnum = PINSEL_PORT_0;
	pinADC_CH2.Pinnum = PINSEL_PIN_25;
	pinADC_CH2.Funcnum = PINSEL_FUNC_1;
	pinADC_CH2.Pinmode = PINSEL_PINMODE_TRISTATE;
	pinADC_CH2.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&pinADC_CH2);

	PINSEL_CFG_Type pinADC_CH4;
	pinADC_CH4.Portnum = PINSEL_PORT_1;
	pinADC_CH4.Pinnum = PINSEL_PIN_30;
	pinADC_CH4.Funcnum = PINSEL_FUNC_3;
	pinADC_CH4.Pinmode = PINSEL_PINMODE_TRISTATE;
	pinADC_CH4.OpenDrain = PINSEL_PINMODE_NORMAL;
	PINSEL_ConfigPin(&pinADC_CH4);

	return;
}

void cfgADC(void){
	ADC_Init(LPC_ADC, 40000);
	ADC_BurstCmd(LPC_ADC, ENABLE);
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_2);
	ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_4);
}

void cfgGPDMA(void){
	GPDMA_LLI_Type cfgADC_LLI0_CH0;
	GPDMA_Channel_CFG_Type cfgADC_MEM_CH0;
	GPDMA_LLI_Type cfgADC_LLI0_CH1;
	GPDMA_Channel_CFG_Type cfgADC_MEM_CH1;

	NVIC_DisableIRQ(DMA_IRQn);
	GPDMA_Init();

	cfgADC_LLI0_CH0.SrcAddr = (uint32_t)&(LPC_ADC->ADDR2);
	cfgADC_LLI0_CH0.DestAddr = (uint32_t)ADC_BUFFER_START0;
	cfgADC_LLI0_CH0.NextLLI = &cfgADC_LLI0_CH0;
	cfgADC_LLI0_CH0.Control = (BUFFER_SIZE<<0)|(2<<18)|(2<<21)&~(1<<26)|(1<<27);

	cfgADC_MEM_CH0.ChannelNum = 0;
	cfgADC_MEM_CH0.TransferSize = BUFFER_SIZE;
	cfgADC_MEM_CH0.SrcMemAddr = 0;
	cfgADC_MEM_CH0.DestMemAddr = (uint32_t)ADC_BUFFER_START0;
	cfgADC_MEM_CH0.TransferType = GPDMA_TRANSFERTYPE_P2M;
	cfgADC_MEM_CH0.SrcConn = GPDMA_CONN_ADC;
	cfgADC_MEM_CH0.DestConn = 0;
	cfgADC_MEM_CH0.DMALLI = (uint32_t)&cfgADC_LLI0_CH0;

	cfgADC_LLI0_CH1.SrcAddr = (uint32_t)&(LPC_ADC->ADDR4);
	cfgADC_LLI0_CH1.DestAddr = (uint32_t)ADC_BUFFER_START1;
	cfgADC_LLI0_CH1.NextLLI = &cfgADC_LLI0_CH1;
	cfgADC_LLI0_CH1.Control = (BUFFER_SIZE<<0)|(2<<18)|(2<<21)&~(1<<26)|(1<<27);

	cfgADC_MEM_CH1.ChannelNum = 0;
	cfgADC_MEM_CH1.TransferSize = BUFFER_SIZE;
	cfgADC_MEM_CH1.SrcMemAddr = 0;
	cfgADC_MEM_CH1.DestMemAddr = (uint32_t)ADC_BUFFER_START1;
	cfgADC_MEM_CH1.TransferType = GPDMA_TRANSFERTYPE_P2M;
	cfgADC_MEM_CH1.SrcConn = GPDMA_CONN_ADC;
	cfgADC_MEM_CH1.DestConn = 0;
	cfgADC_MEM_CH1.DMALLI = (uint32_t)&cfgADC_LLI0_CH1;

	GPDMA_Setup(&cfgADC_MEM_CH0);
	GPDMA_Setup(&cfgADC_MEM_CH1);
}