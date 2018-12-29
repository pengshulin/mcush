/* MCUSH designed by Peng Shulin, all rights reserved. */
#include "mcush.h"

uint32_t idle_counter;

void vApplicationTickHook(void)  __attribute__( ( weak ) );
void vApplicationTickHook(void) 
{
}

void vApplicationIdleHook(void) __attribute__( ( weak ) );
void vApplicationIdleHook(void) 
{
    idle_counter++;
}

void vApplicationMallocFailedHook(void) __attribute__( ( weak ) );
void vApplicationMallocFailedHook(void) 
{
    halt("malloc fail");
}

void vApplicationStackOverflowHook( xTaskHandle xTask, signed portCHAR *pcTaskName ) __attribute__( ( weak ) );
void vApplicationStackOverflowHook( xTaskHandle xTask, signed portCHAR *pcTaskName )
{
    halt("stack overflow");
}

