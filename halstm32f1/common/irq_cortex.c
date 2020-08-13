#include "mcush.h"

void NMI_Handler(void);
void HardFault_Handler(void);
void MemManage_Handler(void);
void BusFault_Handler(void);
void UsageFault_Handler(void);
void DebugMon_Handler(void);



__weak void NMI_Handler(void)
{
    while(1);
}


__weak void HardFault_Handler(void)
{
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
    while (1);
}


