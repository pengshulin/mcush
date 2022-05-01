#include "hal.h"

#define HAL_ADC_NUM     8

    
static uint8_t _adc_in_use;

static float _adc_value[HAL_ADC_NUM];


void hal_adc_init( void )
{
    int i;
    for( i=0; i<HAL_ADC_NUM; i++ )
        _adc_value[i] = 0.0f;
    _adc_in_use = 1;
}

 
void hal_adc_deinit( void )
{
    _adc_in_use = 0;
}


float hal_adc_get( int index )
{
    return _adc_value[index];
}


int hal_adc_get_num( void )
{
    return HAL_ADC_NUM;
}


int hal_adc_in_use( void )
{
    return _adc_in_use;
}

