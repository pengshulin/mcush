#ifndef _TASK_ADC_H_
#define _TASK_ADC_H_

#define TASK_ADC_STACK_SIZE  (1024 / sizeof(portSTACK_TYPE))
#define TASK_ADC_PRIORITY    (MCUSH_PRIORITY + 1)
#define TASK_ADC_QUEUE_SIZE  (4)

extern void task_adc_init(void);

#endif

