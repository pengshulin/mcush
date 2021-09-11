/*
 * The Clear BSD License
 * Copyright (c) 2017, NXP Semiconductors, Inc.
 * All rights reserved.
 *
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted (subject to the limitations in the disclaimer below) provided
 *  that the following conditions are met:
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

#ifndef _FSL_INPUTMUX_CONNECTIONS_
#define _FSL_INPUTMUX_CONNECTIONS_

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Component ID definition, used by tools. */
#ifndef FSL_COMPONENT_ID
#define FSL_COMPONENT_ID "platform.drivers.inputmux_connections"
#endif

/*!
 * @addtogroup inputmux_driver
 * @{
 */

/*!
 * @name Input multiplexing connections
 * @{
 */

/*! @brief Periphinmux IDs */
#define DMA_OTRIG_PMUX_ID 0x00U
#define SCT0_PMUX_ID 0x20U
#define DMA_TRIG0_PMUX_ID 0x40U
#define PMUX_SHIFT 20U

/*! @brief INPUTMUX connections type */
typedef enum _inputmux_connection_t
{
    /*!< DMA OTRIG. */
    kINPUTMUX_DmaChannel0TrigoutToTriginChannels = 0U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel1TrigoutToTriginChannels = 1U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel2TrigoutToTriginChannels = 2U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel3TrigoutToTriginChannels = 3U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel4TrigoutToTriginChannels = 4U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel5TrigoutToTriginChannels = 5U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel6TrigoutToTriginChannels = 6U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel7TrigoutToTriginChannels = 7U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel80TrigoutToTriginChannels = 8U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel9TrigoutToTriginChannels = 9U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel10TrigoutToTriginChannels = 10U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel11TrigoutToTriginChannels = 11U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel12TrigoutToTriginChannels = 12U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel13TrigoutToTriginChannels = 13U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel14TrigoutToTriginChannels = 14U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel15TrigoutToTriginChannels = 15U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel16TrigoutToTriginChannels = 16U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel17TrigoutToTriginChannels = 17U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel18TrigoutToTriginChannels = 18U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel19TrigoutToTriginChannels = 19U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel20TrigoutToTriginChannels = 20U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel21TrigoutToTriginChannels = 21U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel22TrigoutToTriginChannels = 22U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel23TrigoutToTriginChannels = 23U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaChannel24TrigoutToTriginChannels = 24U + (DMA_OTRIG_PMUX_ID << PMUX_SHIFT),

    /*!< SCT INMUX. */
    kINPUTMUX_SctPin0ToSct0 = 0U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_SctPin1ToSct0 = 1U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_SctPin2ToSct0 = 2U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_SctPin3ToSct0 = 3U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_AdcThcmpIrqToSct0 = 4U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_AcmpOToSct0 = 5U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_T0Mat2ToSct0 = 6U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_GpiointBmatchToSct0 = 7U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_ArmTxevToSct0 = 8U + (SCT0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DebugHaltedToSct0 = 9U + (SCT0_PMUX_ID << PMUX_SHIFT),

    /*!< DMA ITRIG. */
    kINPUTMUX_AdcASeqaIrqToDma = 0U + (DMA_TRIG0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_AdcBSeqbIrqToDma = 1U + (DMA_TRIG0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_SctDma0ToDma = 2U + (DMA_TRIG0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_SctDma1ToDma = 3U + (DMA_TRIG0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_AcmpOToDma = 4U + (DMA_TRIG0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_PinInt4ToDma = 5U + (DMA_TRIG0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_PinInt5ToDma = 6U + (DMA_TRIG0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_PinInt6ToDma = 7U + (DMA_TRIG0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_PinInt7ToDma = 8U + (DMA_TRIG0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_T0DmareqM0ToDma = 9U + (DMA_TRIG0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_T0DmareqM1ToDma = 10U + (DMA_TRIG0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaTriggerMux0ToDma = 11U + (DMA_TRIG0_PMUX_ID << PMUX_SHIFT),
    kINPUTMUX_DmaTriggerMux1ToDma = 12U + (DMA_TRIG0_PMUX_ID << PMUX_SHIFT),
} inputmux_connection_t;

/*@}*/

#endif /* _FSL_INPUTMUX_CONNECTIONS_ */
