#include "hal.h"



void hal_reset(void)
{
    IWDG_ReloadCounter();
    //NVIC_SystemReset();

    /* reset all peripheral registers if necessary */
    USART1->CR1 = USART1->CR2 = USART1->CR3 = 0;
    USART2->CR1 = USART2->CR2 = USART2->CR3 = 0;
    USART3->CR1 = USART3->CR2 = USART3->CR3 = 0;

    /* reset to entry point */
    portENTER_CRITICAL();
    //__asm volatile
    //( 
    //    "   ldr r0,  =0x08000000 \n"
    //    "   ldr r1,  =0 \n"
    //    "   ldr r2,  =0 \n"
    //    "   ldr r3,  =0 \n"
    //    "   ldr r4,  =0 \n"
    //    "   ldr r5,  =0 \n"
    //    "   ldr r6,  =0 \n"
    //    "   ldr r7,  =0 \n"
    //    "   ldr r8,  =0 \n"
    //    "   ldr r9,  =0 \n"
    //    "   ldr r10, =0 \n"
    //    "   ldr r11, =0 \n"
    //    "   ldr r12, =0 \n"
    //    "   ldr r13, [r0] \n"
    //    "   ldr r14, =0xFFFFFFFF \n"
    //    "   ldr r0,  =0x08000004 \n"
    //    "   ldr r15, [r0] \n"
    //);

    /* should not run to here */
    while(1);
}

