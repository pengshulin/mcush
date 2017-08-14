/** 
  ******************************************************************************
  * @file    QSPI/QSPI_ReadWrite_IT/Src/stm32f4xx_it.c
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

/** @addtogroup QSPI_ReadWrite_IT
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern __IO int8_t ubRxIndex,ubTxIndex;
extern uint8_t aRxBuffer[];
extern uint8_t aTxBuffer[];
extern __IO uint8_t CmdCplt, RxCplt, TxCplt, StatusMatch;
extern uint8_t NbrOfDataToTransfer;

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
  *@brief  This function handles QSPI interrupt request.
  * @param  None
  * @retval None
  */
void QUADSPI_IRQHandler(void)
{  
  /* QSPI Fifo Threshold interrupt occurred ----------------------------------*/
  if(QSPI_GetITStatus(QSPI_IT_FT) != RESET)
  {
    if(QSPI_GetFMode() == QSPI_ComConfig_FMode_Indirect_Write)
    {
      /* Transmission process */
      ubTxIndex=0;
      while(QSPI_GetFlagStatus(QSPI_FLAG_FT))
      {
        if(ubTxIndex < NbrOfDataToTransfer)
        {
          /* Fill the FIFO until it is full */
          QSPI_SendData8(aTxBuffer[ubTxIndex]);
          ubTxIndex++;
        }
        else
        {
          QSPI_AbortRequest();
          /* No more data available for the transfer */
          break;
        }
      }
    }
    else if(QSPI_GetFMode() == QSPI_ComConfig_FMode_Indirect_Read)
    {
      ubRxIndex=0;
      /* Receiving Process */
      while(QSPI_GetITStatus(QSPI_IT_FT))
      {
        if(ubRxIndex < NbrOfDataToTransfer)
        {
          /* Read the FIFO until it is empty */
          aRxBuffer[ubRxIndex] = QSPI_ReceiveData8();
          ubRxIndex++;
        }
        else
        {
          /* All data have been received for the transfer */
          QSPI_AbortRequest();
          break;
        }
      }
    }   
  } 
  /* QSPI Transfer Complete interrupt occurred -------------------------------*/
  if(QSPI_GetITStatus(QSPI_IT_TC) != RESET)
  {
    /* Clear interrupt */
    QSPI_ClearITPendingBit(QSPI_IT_TC);
    
    /* Disable the QSPI FIFO Threshold, Transfer Error and Transfer complete Interrupts */
    QSPI_ITConfig(QSPI_IT_TC | QSPI_IT_TE | QSPI_IT_FT,DISABLE);
    
    /* Transfer complete callback */
    if(QSPI_GetFMode() == QSPI_ComConfig_FMode_Indirect_Write)
    {
      /* Clear Busy bit */
      QSPI_AbortRequest();
      
      /* TX Complete callback */
      TxCplt++;
      CmdCplt++;
    }
    else if(QSPI_GetFMode() == QSPI_ComConfig_FMode_Indirect_Read)
    {
      while(QSPI_GetFIFOLevel() != 0)
      {
        if(ubRxIndex < NbrOfDataToTransfer)
        {
          /* Read the the last data received in the FIFO until it is empty */
          aRxBuffer[ubRxIndex] = QSPI_ReceiveData8();
          ubRxIndex++;          
        }
        else
        {
          /* All data have been received for the transfer */
          QSPI_AbortRequest();
          break;
        }
      }
      /* Workaround - Extra data written in the FIFO at the end of a read transfer */
      QSPI_AbortRequest();
      
      /* RX Complete callback */
      RxCplt ++;
      CmdCplt++;
    }
    else if(QSPI_GetFIFOLevel() != 0)
    {
      /* Command Complete callback */
      CmdCplt++;
    }
  }  
  /* QSPI Status Match interrupt occurred ------------------------------------*/
  if(QSPI_GetITStatus(QSPI_IT_SM) != RESET)
  {
    /* Clear flag */
    QSPI_ClearITPendingBit(QSPI_IT_SM);
    
    /* Disable the QSPI FIFO Threshold, Transfer Error and Status Match Interrupts */
    QSPI_ITConfig(QSPI_IT_SM | QSPI_IT_FT | QSPI_IT_TE, DISABLE);
    
    /* Status match callback */
    StatusMatch++;
  }  
  /* QSPI Transfer Error interrupt occurred ----------------------------------*/
  if(QSPI_GetITStatus(QSPI_IT_TE) != RESET)
  {
    /* Clear interrupt */
    QSPI_ClearITPendingBit(QSPI_IT_TE);
    
    /* Disable all the QSPI Interrupts */
    QSPI_ITConfig(QSPI_IT_SM | QSPI_IT_TC | QSPI_IT_TE | QSPI_IT_FT, DISABLE);
  }
  
  /* QSPI Timeout interrupt occurred -----------------------------------------*/
  if(QSPI_GetITStatus(QSPI_IT_TO) != RESET)
  {
    /* Clear interrupt */
    QSPI_ClearITPendingBit(QSPI_IT_TO);
  }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
