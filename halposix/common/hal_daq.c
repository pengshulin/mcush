#include "mcush.h"


#define HAL_DAQ_NUM      8
#define DMA_LENGTH_MAX   0xFFFF
#define DAQ_FREQ_MIN     10
#define DAQ_FREQ_MAX     512000

static char *_mem;
static int _sampling_length, _channel_num;
extern float adc_int12_to_voltage(int val);
extern int _ADC_REG_RANK_LIST[];
extern int _ADC_REG_SEQ_SCAN_ENABLE_RANKS_LIST[];



int hal_daq_init( int freq, int sampling_length, int channel_num, int channel_mask, void *buf )
{
    if( buf == NULL )
        return 0;
    if( channel_num <= 0 || channel_num > HAL_DAQ_NUM )
        return 0;
    if( freq < DAQ_FREQ_MIN || freq > DAQ_FREQ_MAX )
        return 0;
    if( sampling_length * channel_num > DMA_LENGTH_MAX  )
        return 0;
    if( channel_num != test_bit1_num(channel_mask) )
        return 0;
    
    _mem = buf;
    _sampling_length = sampling_length;
    _channel_num = channel_num;

    return 1;
}

 
void hal_daq_deinit( void )
{
}


float hal_daq_get( int index )
{
    (void)index;
    return 0;
}


int hal_daq_get_num( void )
{
    return HAL_DAQ_NUM;
}


int hal_daq_start( void )
{
    return 1;
}


int hal_daq_stop( void )
{
    return 1;
}


int hal_daq_reset( void )
{
    hal_daq_stop(); 
    return 1;
}


int hal_daq_done(void)
{
    return 0;
}

