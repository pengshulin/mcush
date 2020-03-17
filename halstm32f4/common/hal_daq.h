/* MCUSH designed by Peng Shulin, all rights reserved. */
#ifndef __HAL_DAQ_H__
#define __HAL_DAQ_H__


/* daq module */
int hal_daq_init( int freq, int sampling_length, int channel_num, int channel_mask, void *buf );
void hal_daq_deinit(void);
int hal_daq_start(void);
int hal_daq_stop(void);
int hal_daq_reset(void);
int hal_daq_done(void);


#endif
