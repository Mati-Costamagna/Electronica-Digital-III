/*
 * Por un pin del ADC del microcontrolador LPC1769 ingresa un tension de rango dinamico 0 a 3.3[V] proviente de un sensor de temperatura.
 * Debido a la baja tasa de variacion de la senal, se pide tomar una muestra cada 30[s]. Pasados los 2[min] se debe promediar las ultimas 
 * 4 muestras y en funcion de este valor, tomar una decision sobre una salida digital de la placa:
 *
 * - Si el valor es <1[V] colocar la salida en 0 (0[V])
 * - Si el valor es >=1[V] y <=2[V] sacar el valor del promedio por el DAC
 * - Si el valor es >2[V] colocar la salida en 1 (3.3[V])

*/


#include "lpc17xx.h"
#include "lpc17xx_adc.h"
#include "lpc17xx_timer.h"

#define SAMPLES_NUM (4)
#define MATCH_VALUE_15_SEC (15)
#define MATCH_VALUE_2_MIN (120)
#define UMBRAL_1V (4096/3.3)
#define UMBRAL_2V (2*4096/3.3)

volatile uint32_t average = 0;
volatile uint32_t samples[SAMPLES_NUM];

void cfgPBC(void);
void cfgTimer(void);
void cfgADC(void);
void cfgDAC(void);

int main(void){

    while(1){};

    return;
}

void cfgPBC(void){
    PINSEL_CFG_Type pinADC;
    pinADC.Portnum = PINSEL_PORT_0;
    pinADC.Pinnum = PINSEL_PIN_23;
    pinADC.Funcnum = PINSEL_FUNC_1;
    pinADC.Pinmode = PINSEL_PINMODE_TRISTATE;
    pinADC.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin(&pinADC);

    PINSEL_CFG_Type pinOutput;
    pinOutput.Portnum = PINSEL_PORT_0;
    pinOutput.Pinnum = PINSEL_PORT_0;
    pinOutput.Funcnum = PINSEL_FUNC_0;
    pinOutput.Pinmode = PINSEL_PINMODE_TRISTATE;
    pinOutput.OpenDrain = PINSEL_PINMODE_NORMAL;
    GPIO_SetDir(PORT_0, PIN_0, OUTPUT);
    PINSEL_ConfigPin(&pinOutput);
    
    PINSEL_CFG_Type pinDAC;
    pinDAC.Portnum = PINSEL_PORT_0;
    pinDAC.Pinnum = PINSEL_PIN_26;
    pinDAC.Funcnum = PINSEL_FUNC_2;
    pinDAC.Pinmode = PINSEL_PINMODE_TRISTATE;
    pinDAC.OpenDrain = PINSEL_PINMODE_NORMAL;
    PINSEL_ConfigPin(&pinDAC);

    return;
}

void cfgADC(void){
    ADC_Init(LPC_ADC, 20000); //Frecuencia maxima 200kHz
    ADC_BurstCmd(LPC_ADC, DISABLE);
    ADC_StartCmd(LPC_ADC, ADC_START_ON_MAT01);
    ADC_ChannelCmd(LPC_ADC, ADC_CHANNEL_0, ENABLE);
    ADC_EdgeStartConfig(LPC_ADC, START_ON_FALLING_EDGE);
    ADC_IntConfig(LPC_ADC, ADC_ADINTEN0, ENABLE);

    NVIC_EnableIRQ(ADC_IRQn);

    return;
}

void cfgTimer(void){
    TIM_TIMERCFG_Type timerMode;
    timerMode.PrescaleOption = TIM_PRESCALE_USVAL;
    timerMode.PrescaleValue = 1000000; //Base de tiempo de desborde de 1s

    TIM_MATCHCFG_Type timerMAT01;
    timerMAT01.MatchChannel = 1;
    timerMAT01.IntOnMatch = DISABLE;
    timerMAT01.StopOnMatch = DISABLE;
    timerMAT01.ResetOnMatch = ENABLE;
    timerMAT01.ExtMatchOutputType = TIM_EXTMATCH_TOGGLE;
    timerMAT01.MatchValue = (uint32_t)MATCH_VALUE_15_SEC;

    TIM_MATCHCFG_Type timerMAT00;
    timerMAT00.MatchChannel = 0;
    timerMAT00.IntOnMatch = ENABLE;
    timerMAT00.StopOnMatch = DISABLE;
    timerMAT00.ResetOnMatch = ENABLE;
    timerMAT00.ExtMatchOutputType = TIM_EXTMATCH_NOTHING;
    timerMAT00.MatchValue = (uint32_t)MATCH_VALUE_2_MIN;

    TIM_Init(LPC_TIM0, TIM_TIMER_MODE, &timerMode);
    TIM_ConfigMatch(LPC_TIM0, &timerMAT01);
    TIM_ConfigMatch(LPC_TIM0, &timerMAT00);
    TIM_Cmd(LPC_TIM0, ENABLE);

    NVIC_EnableIRQ(TIMER0_IRQn);

    return;
}

void cfgDAC(void){
    DAC_Init(LPC_DAC);
    DAC_SetBias(DAC_MAX_CURRENT_350uA);
}

void ADC_IRQHandler(void){
    static uint8_t idx = 0;
    samples[idx] = ADC_ChannelGetData(LPC_ADC, ADC_CHANNEL_0);
    idx = (idx + 1) % SAMPLES_NUM;
    return;
}

void TIMER0_IRQHandler(void){
    if(TIM_GetIntStatus(LPC_TIM0, TIM_MR0_INT)){
        average = 0;

        for(uint8_t inte = 0; inte < SAMPLES_NUM; inte++){
            average += samples[inte];
        }
        average /= SAMPLES_NUM;

        if(average < (uint32_t) UMBRAL_1V){
            FIO_ByteSetValue(PINSEL_PORT_0, PINSEL_PIN_0, 0);
        }else if(average > (uint32_t) UMBRAL_2V){
            FIO_ByteSetValue(PINSEL_PORT_0, PINSEL_PIN_0, 1);
        }else{
            DAC_UpdateValue(LPC_DAC, average)
        }    
    }
    
    TIM_ClearIntPending(LPC_TIM0, TIM_MR0_INT);
}