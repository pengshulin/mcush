/*
 * The Clear BSD License
 * Copyright 2017 NXP
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 * that the following conditions are met:
 *
 * o Redistributions of source code must retain the above copyright notice, this list
 *   of conditions and the following disclaimer.
 *
 * o Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 *
 * o Neither the name of the copyright holder nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * NO EXPRESS OR IMPLIED LICENSES TO ANY PARTY'S PATENT RIGHTS ARE GRANTED BY THIS LICENSE.
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "fsl_capt.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/

/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.capt"
#endif

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
/*!
 * @brief Get the CAPT instance from the peripheral base address.
 *
 * @param base CAPT peripheral base address.
 * @return CAPT instance.
 */
static uint32_t CAPT_GetInstance(CAPT_Type *base);

/*******************************************************************************
 * Variables
 ******************************************************************************/
/* Array of CAPT peripheral base address. */
static CAPT_Type *const s_captBases[] = CAPT_BASE_PTRS;
#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
/* Clock name of CAPT. */
static const clock_ip_name_t s_captClock[] = CAPT_CLOCKS;
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

/*******************************************************************************
 * Codes
 ******************************************************************************/
static uint32_t CAPT_GetInstance(CAPT_Type *base)
{
    uint32_t instance = 0U;
    uint32_t captArrayCount = (sizeof(s_captBases) / sizeof(s_captBases[0]));

    /* Find the instance index from base address mappings. */
    for (instance = 0; instance < captArrayCount; instance++)
    {
        if (s_captBases[instance] == base)
        {
            break;
        }
    }

    return instance;
}
void CAPT_Init(CAPT_Type *base, const capt_config_t *config)
{
    assert(config != NULL);

    uint32_t tmp32;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Open clock gate. */
    CLOCK_EnableClock(s_captClock[CAPT_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */

    /* Write CTRL register. */
    tmp32 = base->CTRL & (CAPT_CTRL_POLLMODE_MASK | CAPT_CTRL_TYPE_MASK 
#if defined(FSL_FEATURE_CAPT_HAS_CTRL_DMA) && FSL_FEATURE_CAPT_HAS_CTRL_DMA
                          | CAPT_CTRL_DMA_MASK
#endif /* FSL_FEATURE_CAPT_HAS_CTRL_DMA */
                            );
    tmp32 |= CAPT_CTRL_TRIGGER(config->triggerMode) | CAPT_CTRL_FDIV(config->clockDivider) |
             CAPT_CTRL_XPINUSE(config->XpinsMode) | CAPT_CTRL_XPINSEL(config->enableXpins);
    if (config->enableWaitMode)
    {
        tmp32 |= CAPT_CTRL_WAIT_MASK;
    }
    /* Before writing into CTRL register, INCHANGE(bit 15)should equal '0'. */
    while (CAPT_CTRL_INCHANGE_MASK == (CAPT_CTRL_INCHANGE_MASK & base->CTRL))
    {
    }
    base->CTRL = tmp32;

    /* Write POLL_TCNT register. */
    tmp32 = base->POLL_TCNT & CAPT_POLL_TCNT_TCNT_MASK;
    tmp32 |= CAPT_POLL_TCNT_TOUT(config->timeOutCount) | CAPT_POLL_TCNT_POLL(config->pollCount) |
             CAPT_POLL_TCNT_MDELAY(config->mDelay) | CAPT_POLL_TCNT_RDELAY(config->rDelay);
    if (config->enableTouchLower)
    {
        tmp32 |= CAPT_POLL_TCNT_TCHLOW_ER_MASK;
    }
    base->POLL_TCNT = tmp32;
}

void CAPT_Deinit(CAPT_Type *base)
{
    /* Make CAPT inactive. */
    /* Before writing into CTRL register, INCHANGE(bit 15)should equal '0'. */
    while (CAPT_CTRL_INCHANGE_MASK == (CAPT_CTRL_INCHANGE_MASK & base->CTRL))
    {
    }
    base->CTRL &= ~CAPT_CTRL_POLLMODE_MASK;

#if !(defined(FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL) && FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL)
    /* Disable clock gate. */
    CLOCK_DisableClock(s_captClock[CAPT_GetInstance(base)]);
#endif /* FSL_SDK_DISABLE_DRIVER_CLOCK_CONTROL */
}

void CAPT_GetDefaultConfig(capt_config_t *config)
{
    assert(NULL != config);

    config->enableWaitMode = false;
    config->enableTouchLower = true;
    config->clockDivider = 15U;
    config->timeOutCount = 12U;
    config->pollCount = 0U;
    config->enableXpins = 0U;
    config->triggerMode = kCAPT_YHPortTriggerMode;
    config->XpinsMode = kCAPT_InactiveXpinsDrivenLowMode;
    config->mDelay = kCAPT_MeasureDelayNoWait;
    config->rDelay = kCAPT_ResetDelayWait9FCLKs;
}

void CAPT_SetPollMode(CAPT_Type *base, capt_polling_mode_t mode)
{
    /* Before writing into CTRL register, INCHANGE(bit 15)should equal '0'. */
    while (CAPT_CTRL_INCHANGE_MASK == (CAPT_CTRL_INCHANGE_MASK & base->CTRL))
    {
    }

    base->CTRL &= ~CAPT_CTRL_POLLMODE_MASK;
    base->CTRL |= CAPT_CTRL_POLLMODE(mode);
}

#if defined(FSL_FEATURE_CAPT_HAS_CTRL_DMA) && FSL_FEATURE_CAPT_HAS_CTRL_DMA
void CAPT_EnableDMA(CAPT_Type *base, capt_dma_mode_t mode)
{
    /* Before writing into CTRL register, INCHANGE(bit 15)should equal '0'. */
    while (CAPT_CTRL_INCHANGE_MASK == (CAPT_CTRL_INCHANGE_MASK & base->CTRL))
    {
    }
    base->CTRL = (base->CTRL & ~CAPT_CTRL_DMA_MASK) | CAPT_CTRL_DMA(mode);
}

void CAPT_DisableDMA(CAPT_Type *base)
{
    /* Before writing into CTRL register, INCHANGE(bit 15)should equal '0'. */
    while (CAPT_CTRL_INCHANGE_MASK == (CAPT_CTRL_INCHANGE_MASK & base->CTRL))
    {
    }
    base->CTRL &= ~CAPT_CTRL_DMA_MASK;
}
#endif /*FSL_FEATURE_CAPT_HAS_CTRL_DMA*/

bool CAPT_GetTouchData(CAPT_Type *base, capt_touch_data_t *data)
{
    assert(NULL != data);

    uint32_t tmp32;

    tmp32 = base->TOUCH;

    if (CAPT_TOUCH_CHANGE_MASK == (CAPT_TOUCH_CHANGE_MASK & tmp32))
    {
        return false;
    }
    if (CAPT_TOUCH_ISTOUCH_MASK == (CAPT_TOUCH_ISTOUCH_MASK & tmp32))
    {
        data->yesTouch = true;
    }
    else
    {
        data->yesTouch = false;
    }
    if (CAPT_TOUCH_ISTO_MASK == (CAPT_TOUCH_ISTO_MASK & tmp32))
    {
        data->yesTimeOut = true;
    }
    else
    {
        data->yesTimeOut = false;
    }
    data->XpinsIndex = (CAPT_TOUCH_XVAL_MASK & tmp32) >> CAPT_TOUCH_XVAL_SHIFT;
    data->sequenceNumber = (CAPT_TOUCH_SEQ_MASK & tmp32) >> CAPT_TOUCH_SEQ_SHIFT;
    data->count = (CAPT_TOUCH_COUNT_MASK & tmp32) >> CAPT_TOUCH_COUNT_SHIFT;

    return true;
}
