#include "hal.h"

/* Hardware connection:
   ----------------------------- 
   (MCU)
               (used)
   PA0 ------> HAL_ADC123_IN0
   PA1 ------> HAL_ADC123_IN1
   PA2 ------> HAL_ADC123_IN2
   PA3 ------> HAL_ADC123_IN3
               (not used)
   PA4 ------> HAL_ADC12_IN4
   PA5 ------> HAL_ADC12_IN5
   PA6 ------> HAL_ADC123_IN6
   PA7 ------> HAL_ADC123_IN7
   PB0 ------> HAL_ADC123_IN8
   PB1 ------> HAL_ADC123_IN9
   PB2         NOT SUPPORT
   PB3         NOT SUPPORT
   PC0 ------> HAL_ADC123_IN10
   PC1 ------> HAL_ADC123_IN11
   PC2 ------> HAL_ADC123_IN12
   PC3 ------> HAL_ADC123_IN13
   PE[0:15]    NOT SUPPORT
   ----------------------------- 
 */

#ifndef HAL_ADC_DEFINE 
    #define HAL_ADC_RCC_ADC_ENABLE_CMD     LL_APB2_GRP1_EnableClock
    #define HAL_ADC_RCC_ADC_ENABLE_BIT     LL_APB2_GRP1_PERIPH_ADC1
    #define HAL_ADC_RCC_DMA_ENABLE_CMD     LL_AHB1_GRP1_EnableClock
    #define HAL_ADC_RCC_DMA_ENABLE_BIT     LL_AHB1_GRP1_PERIPH_DMA1
    #define HAL_ADC_NUM                    4
    #define HAL_ADC_PORT                   GPIOA
    #define HAL_ADC_PINS                   (GPIO_PIN_0|GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3)
    #define HAL_ADCx                       ADC1
    #define HAL_ADCx_DATA_REG_ADDR         ((uint32_t)0x4001244C)
    #define HAL_DMAx                       DMA1
    //#define HAL_DMA_Streamy                LL_DMA_STREAM_0
    #define HAL_DMA_Channelz               LL_DMA_CHANNEL_1
#endif

    
static uint8_t _adc_in_use;

static uint16_t _adc_value[HAL_ADC_NUM];


const int _ADC_REG_RANK_LIST[] = {
    LL_ADC_REG_RANK_1,
    LL_ADC_REG_RANK_2,
    LL_ADC_REG_RANK_3,
    LL_ADC_REG_RANK_4,
    LL_ADC_REG_RANK_5,
    LL_ADC_REG_RANK_6,
    LL_ADC_REG_RANK_7,
    LL_ADC_REG_RANK_8,
    };

const int _ADC_REG_SEQ_SCAN_ENABLE_RANKS_LIST[] = {
    LL_ADC_REG_SEQ_SCAN_DISABLE,
    LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS,
    LL_ADC_REG_SEQ_SCAN_ENABLE_3RANKS,
    LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS,
    LL_ADC_REG_SEQ_SCAN_ENABLE_5RANKS,
    LL_ADC_REG_SEQ_SCAN_ENABLE_6RANKS,
    LL_ADC_REG_SEQ_SCAN_ENABLE_7RANKS,
    LL_ADC_REG_SEQ_SCAN_ENABLE_8RANKS,
    };

 
float adc_int12_to_voltage(int val)
{
    float ret = val;
#if defined(HAL_ADC_VREF)
    ret = HAL_ADC_VREF * ret / 4096.0f;
#else
    ret = 3.3f * ret / 4096.0f;
#endif
    return ret;
}


/* NOTE:
   configure adc as multi-channel continuous scan mode, all available
   channels are organized in regular group and sequencely converted, 
   results will be gathered by dma looply and saved into adc_value array
   all steps are done automatically without CPU intervention
   set sampling cycles bigger for stable voltage inputs
   after inited, unused analog pins can be set back for digital control
   after inited, daq module can be simultaneously used for waveform capture
 */
void hal_adc_init( void )
{
    LL_GPIO_InitTypeDef gpio_init;
    //LL_ADC_CommonInitTypeDef adc_common_init;
    LL_ADC_InitTypeDef adc_init;
    LL_ADC_REG_InitTypeDef adc_reg_init;
    LL_DMA_InitTypeDef dma_init;
    unsigned int i;

    /* pins init */
    gpio_init.Speed = LL_GPIO_SPEED_FREQ_LOW;
    gpio_init.Mode = LL_GPIO_MODE_ANALOG;
    gpio_init.Pull = LL_GPIO_PULL_DOWN;
    gpio_init.OutputType = LL_GPIO_OUTPUT_OPENDRAIN;
    gpio_init.Pin = HAL_ADC_PINS;
    LL_GPIO_Init( HAL_ADC_PORT, &gpio_init );
   
    /* adc init */
    LL_APB2_GRP1_EnableClock( LL_APB2_GRP1_PERIPH_ADC1 );
    LL_RCC_SetADCClockSource( LL_RCC_ADC_CLKSRC_PCLK2_DIV_6);  /* 72M/6=12M */

    LL_ADC_DeInit( HAL_ADCx );
    adc_init.DataAlignment = LL_ADC_DATA_ALIGN_RIGHT;
    //adc_init.Resolution = LL_ADC_RESOLUTION_12B;
    adc_init.SequencersScanMode = LL_ADC_SEQ_SCAN_ENABLE;
    LL_ADC_Init( HAL_ADCx, &adc_init );

    adc_reg_init.TriggerSource = LL_ADC_REG_TRIG_SOFTWARE;
    adc_reg_init.SequencerLength = LL_ADC_REG_SEQ_SCAN_ENABLE_4RANKS;
    adc_reg_init.SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE;
    adc_reg_init.ContinuousMode = LL_ADC_REG_CONV_CONTINUOUS;
    adc_reg_init.DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED; 
    LL_ADC_REG_Init( HAL_ADCx, &adc_reg_init );

    for( i = 0; i < HAL_ADC_NUM; i++ )
    {
        //LL_ADC_SetChannelSamplingTime( HAL_ADCx, __LL_ADC_DECIMAL_NB_TO_CHANNEL(i), LL_ADC_SAMPLINGTIME_1CYCLES_5);
        //LL_ADC_SetChannelSamplingTime( HAL_ADCx, __LL_ADC_DECIMAL_NB_TO_CHANNEL(i), LL_ADC_SAMPLINGTIME_7CYCLES_5);
        //LL_ADC_SetChannelSamplingTime( HAL_ADCx, __LL_ADC_DECIMAL_NB_TO_CHANNEL(i), LL_ADC_SAMPLINGTIME_13CYCLES_5);
        //LL_ADC_SetChannelSamplingTime( HAL_ADCx, __LL_ADC_DECIMAL_NB_TO_CHANNEL(i), LL_ADC_SAMPLINGTIME_28CYCLES_5);
        //LL_ADC_SetChannelSamplingTime( HAL_ADCx, __LL_ADC_DECIMAL_NB_TO_CHANNEL(i), LL_ADC_SAMPLINGTIME_41CYCLES_5);
        //LL_ADC_SetChannelSamplingTime( HAL_ADCx, __LL_ADC_DECIMAL_NB_TO_CHANNEL(i), LL_ADC_SAMPLINGTIME_55CYCLES_5);
        //LL_ADC_SetChannelSamplingTime( HAL_ADCx, __LL_ADC_DECIMAL_NB_TO_CHANNEL(i), LL_ADC_SAMPLINGTIME_71CYCLES_5);
        LL_ADC_SetChannelSamplingTime( HAL_ADCx, __LL_ADC_DECIMAL_NB_TO_CHANNEL(i), LL_ADC_SAMPLINGTIME_239CYCLES_5);
        LL_ADC_REG_SetSequencerRanks( HAL_ADCx, _ADC_REG_RANK_LIST[i], __LL_ADC_DECIMAL_NB_TO_CHANNEL(i) );
        _adc_value[i] = 0;
    }
    LL_ADC_Enable( HAL_ADCx );

    /* calibrate */
    //LL_ADC_ResetCalibration(HAL_ADCx);
    //while(ADC_GetResetCalibrationStatus(HAL_ADCx));
    LL_ADC_StartCalibration(HAL_ADCx);
    while(LL_ADC_IsCalibrationOnGoing(HAL_ADCx));
 
    /* dma */
    HAL_ADC_RCC_DMA_ENABLE_CMD( HAL_ADC_RCC_DMA_ENABLE_BIT );
    //LL_DMA_DisableStream( HAL_DMAx, HAL_DMA_Streamy );
    LL_DMA_DisableChannel( HAL_DMAx, HAL_DMA_Channelz );
    LL_DMA_DeInit( HAL_DMAx, HAL_DMA_Channelz );
    dma_init.Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY;
    dma_init.Mode = LL_DMA_MODE_CIRCULAR;
    dma_init.PeriphOrM2MSrcAddress = HAL_ADCx_DATA_REG_ADDR;
    dma_init.MemoryOrM2MDstAddress = (uint32_t)&_adc_value;
    dma_init.PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT;
    dma_init.MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT;
    dma_init.PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD;
    dma_init.MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD;
    dma_init.NbData = HAL_ADC_NUM;
    //dma_init.Channel = HAL_DMA_Channelz;
    dma_init.Priority = LL_DMA_PRIORITY_MEDIUM;
    //dma_init.FIFOMode = LL_DMA_FIFOMODE_DISABLE;
    //dma_init.FIFOThreshold = LL_DMA_FIFOTHRESHOLD_FULL;
    //dma_init.MemBurst = LL_DMA_MBURST_SINGLE;
    //dma_init.PeriphBurst = LL_DMA_PBURST_SINGLE;
    LL_DMA_Init( HAL_DMAx, HAL_DMA_Channelz, &dma_init );
    //LL_DMA_EnableStream( HAL_DMAx, HAL_DMA_Streamy );
    LL_DMA_EnableChannel( HAL_DMAx, HAL_DMA_Channelz );

    /* start */
    LL_ADC_REG_StartConversionSWStart( HAL_ADCx );

    _adc_in_use = 1;
}

 
void hal_adc_deinit( void )
{
    LL_GPIO_InitTypeDef gpio_init;
 
    gpio_init.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    gpio_init.Mode = LL_GPIO_MODE_INPUT;
    //gpio_init.Alternate = 0;
    gpio_init.Pull = LL_GPIO_PULL_DOWN;
    gpio_init.Pin = HAL_ADC_PINS;
    LL_GPIO_Init( HAL_ADC_PORT, &gpio_init );
   
    LL_ADC_Disable( HAL_ADCx );
    LL_ADC_DeInit( HAL_ADCx );
    //LL_DMA_DisableStream( HAL_DMAx, HAL_DMA_Streamy );
    LL_DMA_DisableChannel( HAL_DMAx, HAL_DMA_Channelz );
    LL_DMA_DeInit( HAL_DMAx, HAL_DMA_Channelz );

    _adc_in_use = 0;
}


float hal_adc_get( int index )
{
    return adc_int12_to_voltage( _adc_value[index] );
}


int hal_adc_get_num( void )
{
    return HAL_ADC_NUM;
}


int hal_adc_in_use( void )
{
    return _adc_in_use;
}

