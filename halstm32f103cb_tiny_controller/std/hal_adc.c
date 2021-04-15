/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "hal.h"

/* Hardware connection:
   ----------------------------- 
   (MCU)                  
   PA0 ------> HAL_ADCx2_IN0
   PA1 ------> HAL_ADCx2_IN1
   PA2 ------> HAL_ADCx2_IN2
   PA3 ------> HAL_ADCx2_IN3
   ----------------------------- 
 */

#ifndef HAL_ADC_DEFINE 
    #define HAL_ADC_RCC_ADC_ENABLE_CMD     RCC_APB2PeriphClockCmd
    #define HAL_ADC_RCC_ADC_ENABLE_BIT     RCC_APB2Periph_ADC1
    #define HAL_ADC_RCC_DMA_ENABLE_CMD     RCC_AHBPeriphClockCmd
    #define HAL_ADC_RCC_DMA_ENABLE_BIT     RCC_AHBPeriph_DMA1
    #define HAL_ADC_NUM                    4
    #define HAL_ADC_PORTS                  {0, 0, 0, 0}
    #define HAL_ADC_PINS                   {0, 1, 2, 3} 
    #define HAL_ADCx                       ADC1
    #define HAL_ADCx_DATA_REG_ADDR         ((uint32_t)0x40012400)
    #define HAL_DMAx                       DMA1 
    #define HAL_DMAx_Channely              DMA1_Channel1
#endif

    

extern const GPIO_TypeDef * const ports[];

static const uint8_t _adc_ports[] = HAL_ADC_PORTS;
static const uint8_t _adc_pins[] = HAL_ADC_PINS;

static uint16_t _adc_value[HAL_ADC_NUM];


float adval_to_voltage(int val)
{
    float ret;
    ret = val;
    ret = 3.3f * ret / 4096.0f;
    return ret;
}


void hal_adc_init( void )
{
    GPIO_InitTypeDef gpio_init;
    ADC_InitTypeDef adc_init;
    DMA_InitTypeDef dma_init;
    int i;

    /* pins init */
    GPIO_StructInit(&gpio_init);
    gpio_init.GPIO_Mode = GPIO_Mode_AIN;
    for( i = 0; i < HAL_ADC_NUM; i++ )
    {
        gpio_init.GPIO_Pin = 1<<(_adc_pins[i]);
        GPIO_Init((GPIO_TypeDef*)ports[_adc_ports[i]], &gpio_init);
    }
   
    /* adc init */ 
    HAL_ADC_RCC_ADC_ENABLE_CMD(HAL_ADC_RCC_ADC_ENABLE_BIT, ENABLE);
    RCC_ADCCLKConfig(RCC_PCLK2_Div6);  /* 72M/6=12M */

    ADC_DeInit( HAL_ADCx );
    adc_init.ADC_Mode = ADC_Mode_Independent;
    adc_init.ADC_ScanConvMode = ENABLE;
    adc_init.ADC_ContinuousConvMode = ENABLE;
    adc_init.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
    adc_init.ADC_DataAlign = ADC_DataAlign_Right;
    adc_init.ADC_NbrOfChannel = HAL_ADC_NUM;
    ADC_Init( HAL_ADCx, &adc_init );
    for( i = 0; i < HAL_ADC_NUM; i++ )
    {
        ADC_RegularChannelConfig(HAL_ADCx, i, i+1, ADC_SampleTime_13Cycles5);
        _adc_value[i] = 0;
    }
    ADC_Cmd( HAL_ADCx, ENABLE );
    ADC_DMACmd( HAL_ADCx, ENABLE );

    /* calibrate */
    ADC_ResetCalibration(HAL_ADCx);
    while(ADC_GetResetCalibrationStatus(HAL_ADCx));
    ADC_StartCalibration(HAL_ADCx);
    while(ADC_GetCalibrationStatus(HAL_ADCx));

    /* dma */
    HAL_ADC_RCC_DMA_ENABLE_CMD( HAL_ADC_RCC_DMA_ENABLE_BIT, ENABLE );
    DMA_Cmd( HAL_DMAx_Channely, DISABLE );
    DMA_DeInit( HAL_DMAx_Channely );
    dma_init.DMA_PeripheralBaseAddr = HAL_ADCx_DATA_REG_ADDR;
    dma_init.DMA_MemoryBaseAddr = (uint32_t)&_adc_value;
    dma_init.DMA_DIR = DMA_DIR_PeripheralSRC;
    dma_init.DMA_BufferSize = HAL_ADC_NUM;
    dma_init.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    dma_init.DMA_MemoryInc = DMA_MemoryInc_Enable;
    dma_init.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
    dma_init.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
    dma_init.DMA_Mode = DMA_Mode_Circular;
    dma_init.DMA_Priority = DMA_Priority_Medium;
    dma_init.DMA_M2M = DMA_M2M_Disable;
    DMA_Init( HAL_DMAx_Channely, &dma_init );
    DMA_Cmd( HAL_DMAx_Channely, ENABLE );

    /* start */
    ADC_SoftwareStartConvCmd( HAL_ADCx, ENABLE );
}

 
void hal_adc_deinit( void )
{
    int i;
    for( i=0; i<HAL_ADC_NUM; i++ )
        hal_gpio_set_input( _adc_ports[i], 1<<(_adc_pins[i]) );
    ADC_DeInit( HAL_ADCx );
    DMA_Cmd( HAL_DMAx_Channely, DISABLE );
    DMA_DeInit( HAL_DMAx_Channely );
}


float hal_adc_get( int index )
{
    return adval_to_voltage( _adc_value[index] );
}


int hal_adc_get_num( void )
{
    return HAL_ADC_NUM;
}

