/**
  ******************************************************************************
  * @file    CEC/CEC_DataExchange/stm32f4xx_it.c
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main Interrupt Service Routines.
  *          This file provides template for all exceptions handler and
  *          peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup CEC_DataExchange
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t rcv_inc  = 0;
uint8_t send_inc = 0;
extern  __IO uint8_t ReceivedFrame;
extern uint8_t  Tab_Rx[CEC_MAX_PAYLOAD];
extern uint8_t  Tab_Tx[CEC_MAX_PAYLOAD-1];
extern uint32_t TxSize;
extern __IO uint8_t HeaderBlockValueToSend;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {
  }
}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{
}

/**
  * @brief  This function handles PendSVC exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{
}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
}

/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f446xx.s).                                             */
/******************************************************************************/

/**
  * @brief  This function handles CEC interrupt request.
  * @param  None
  * @retval None
  */
void CEC_IRQHandler(void)
{
/********************** Reception *******************************************/
  /* Check if a reception error occurred */
  if (CEC->ISR & (CEC_IT_RXACKE | CEC_IT_LBPE | CEC_IT_SBPE | CEC_IT_BRE | CEC_IT_RXOVR))
  {
    CEC->ISR = (CEC_IT_RXACKE | CEC_IT_LBPE | CEC_IT_SBPE | CEC_IT_BRE | CEC_IT_RXOVR);
    ReceivedFrame = 2;
  }

  if (CEC_GetITStatus(CEC_IT_RXBR))
  { 
    /* Check if the byte received is a Header */
    if (rcv_inc == 0)
    {   
      Tab_Rx[rcv_inc] = CEC_ReceiveData();
      rcv_inc++; 
    }
    else /* Receive each byte except header and opcode in the reception buffer */
    {
      Tab_Rx[rcv_inc] = CEC_ReceiveData();
      rcv_inc++;
    }
    CEC_ClearITPendingBit(CEC_IT_RXBR);
  }

  /* Check if the byte received is the last one of the message */
  if (CEC_GetITStatus(CEC_IT_RXEND))
  {  
    ReceivedFrame = 1;

    /* Clear all reception flags */
    CEC_ClearITPendingBit(CEC_IT_RXEND);
    rcv_inc =0;
  }

  /********************** Transmission ****************************************/
  /* Check if a transmission error occurred */
  if (CEC->ISR & (CEC_IT_TXACKE | CEC_IT_TXERR | CEC_IT_TXUDR | CEC_IT_ARBLST))
  {
    CEC->ISR = (CEC_IT_TXACKE | CEC_IT_TXERR | CEC_IT_TXUDR | CEC_IT_ARBLST);
    /* KO */
    /* Turn on LED3 */
  }
  
  /* Check if end of message bit is set in the data to be transmitted */
  if (CEC_GetITStatus(CEC_IT_TXEND))
  {
    CEC_ClearITPendingBit(CEC_IT_TXEND | CEC_IT_TXBR);
    send_inc = 0;
    /* OK */
    /* Turn on LED1 */
  }
  /* Check if data byte has been sent */
  else if (CEC_GetITStatus(CEC_IT_TXBR))
  {
    /* Set EOM bit if the byte to be transmitted is the last one of the Transmit Buffer */
     if (send_inc ==  TxSize )
    {
      CEC_EndOfMessage();
      CEC_SendData(Tab_Tx[send_inc++]);
    }
    else
    {
      /* Put the byte in the TX Buffer */
      CEC_SendData(Tab_Tx[send_inc++]);
    }
    CEC_ClearITPendingBit(CEC_IT_TXBR);
  }
}


/**
  * @brief  This function handles External lines 15 to 10 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI15_10_IRQHandler(void)
{
  if(EXTI_GetITStatus(EXTI_Line13) != RESET)
  {
    Tab_Tx[0] = 0x44;
    Tab_Tx[1] = 0x42;
    TxSize    = 0x03; 
    EXTI_ClearITPendingBit(EXTI_Line13);
  }

  /* Write single Data in the TX Buffer to Transmit through the CEC peripheral */
  CEC_SendData(HeaderBlockValueToSend);

  /* Initiate Message Transmission */
  CEC_StartOfMessage();
}


/**
  * @}
  */

/**
  * @}
  */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
