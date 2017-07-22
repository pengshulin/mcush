/*
 * Copyright (c) 2012, Jens Nielsen
 * All rights reserved.

 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *   * Neither the name of the <organization> nor the
 *     names of its contributors may be used to endorse or promote products
 *     derived from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL JENS NIELSEN BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef OWHAL_H
#define OWHAL_H

#include "stm32f4xx.h"

#define OW_GPIO GPIOA
#define OW_PIN 2

/* the ow hal needs to define the following macros */

/* todo: add disabling of interrupts in ownet */

/**
 * OW_OUTPUT - set one-wire gpio pin to output
 * OW_INPUT - set one-wire gpio pin to input
 */
#define OW_OUTPUT() ( OW_GPIO->MODER = ( ( OW_GPIO->MODER & ~( 3 << (2*OW_PIN) ) ) | ( 0x1 << (2*OW_PIN) ) ) )
#define OW_INPUT() ( OW_GPIO->MODER = ( OW_GPIO->MODER & ~( 3 << (2*OW_PIN) ) ) )

/**
 * OW_HIGH - drive one-wire gpio pin high
 * OW_LOW - drive one-wire gpio pin low
 */
#define OW_HIGH() ( OW_GPIO->BSRRL = 1 << OW_PIN )
#define OW_LOW() ( OW_GPIO->BSRRH = 1 << OW_PIN )

/**
 * OW_READ - read state of one-wire gpio pin
 */
#define OW_READ() ( ( OW_GPIO->IDR >> OW_PIN ) & 1 )

/**
 * A microsecond delay function. Exact precision is not crucial.
 */
static inline __attribute__((always_inline))
void __delay_us(uint32_t us)
{
    /* calculate number of loop iterations */
    /* 168 MHz clock, 3 cycles per loop iteration, minus some overhead */
    //us = ((us * (168 / 3)) - 1);
    //asm volatile("mov r0, %[cycles]\n"
    //             "1:\n"
    //             "subs r0,r0,#1\n"
    //             "bne 1b" :: [cycles] "r" (us) : "r0", "cc" );
    while( us-- )
    {
        __NOP(); __NOP();
        __NOP(); __NOP();
        __NOP(); __NOP();
    }
}

#endif
