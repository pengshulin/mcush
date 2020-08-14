#include "mcush.h"


__weak void NMI_Handler(void)
{
    while(1);
}


__weak void HardFault_Handler(unsigned int *args)
{
    unsigned int r0;
    unsigned int r1;
    unsigned int r2;
    unsigned int r3;
    unsigned int r12;
    unsigned int lr;
    unsigned int pc;
    unsigned int psr;

    r0  = ((unsigned long)args[0]);
    r1  = ((unsigned long)args[1]);
    r2  = ((unsigned long)args[2]);
    r3  = ((unsigned long)args[3]);
    r12 = ((unsigned long)args[4]);
    lr  = ((unsigned long)args[5]);
    pc  = ((unsigned long)args[6]);
    psr = ((unsigned long)args[7]);

    (void)r0;
    (void)r1;
    (void)r2;
    (void)r3;
    (void)r12;
    (void)lr;
    (void)pc;
    (void)psr;

    /* TODO: 
       1. check $lr, determine MSP/PSP:
          0xFFFFFFF9 (-7) - MSP
          0xFFFFFFFD (-3) - PSP
       2. check $msp/$psp and find the fault point 
       3. jump back by 'set $pc=??????'
     */
    while(1);
}


__weak void MemManage_Handler(void)
{
    while(1);
}


__weak void BusFault_Handler(void)
{
    while(1);
}


__weak void UsageFault_Handler(void)
{
    while(1);
}


__weak void DebugMon_Handler(void)
{
    while(1);
}


