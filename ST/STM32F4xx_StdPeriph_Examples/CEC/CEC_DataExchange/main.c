/**
  ******************************************************************************
  * @file    CEC/CEC_DataExchange/main.c 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main program body
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

/** @addtogroup STM32F4xx_StdPeriph_Examples
  * @{
  */

/** @addtogroup CEC_DataExchange
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
typedef enum { FAILED = 0, PASSED = !FAILED} TestStatus;
__IO TestStatus TransferStatus = FAILED;
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t  Tab_Rx[CEC_MAX_PAYLOAD];        /* Received data buffer. Max size = 16 bytes
                                          * header + opcode followed by up to 14 operands */
uint8_t  Tab_Tx[CEC_MAX_PAYLOAD-1];      /* Transmitted data buffer. 
                                          * header is not included in Tab_Tx.
                                          *  Max size = 15 bytes.                                   
                                          *  one opcode followed by up to 14 operands.
                                          *  When payload size = 0, only the header is sent
                                          *  (ping operation) */                                   
__IO uint8_t ReceivedFrame  = 0x0;       /* Set when a reception occurs */
uint16_t NbOfReceivedBytes  = 0x0;       /* Number of received bytes in addition to the header. 
                                          * when a ping message has been received (header 
                                          * only), NbOfReceivedBytes = 0 */
uint32_t TxSize             = 0x0;       /* Number of bytes to transmit in addition to the header. 
                                          * In case of ping operation (only the header sent),
                                          * TxSize = 0 */
uint8_t DestinationAddress;              /* Destination logical address */ 
uint8_t MyOwnAddress;                    /* Sender logical address */
__IO uint8_t HeaderBlockValueToSend = 0; /* Header Block Value */
/* Private function prototypes -----------------------------------------------*/
static void CEC_Config(void);
static void CEC_FlushRxBuffer(void);
static void EXTILine13_15_Config(void);

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{  
  /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       file startup_stm32f446xx.s) before to branch to application main. 
       To reconfigure the default setting of SystemInit() function, refer to
       system_stm32f4xx.c file
     */      
  
  /* CEC device initialization */
#if defined (DEVICE_1)
  DestinationAddress = DEVICE_ADDRESS_2; /* follower address */
  MyOwnAddress       = DEVICE_ADDRESS_1;
#elif defined (DEVICE_2)
  DestinationAddress = DEVICE_ADDRESS_1; /* follower address */
  MyOwnAddress       = DEVICE_ADDRESS_2;
#endif  
  
  /* IP configuration */ 
  CEC_Config();
  
  /* User Button Configuration */
  EXTILine13_15_Config();
   
  /* Build the Header block to send */
  HeaderBlockValueToSend = (uint8_t)((uint32_t)(MyOwnAddress) << CEC_INITIATOR_LSB_POS) | DestinationAddress;
  
  /* Test start */
  
  /* Enter infinite reception loop:
  * Each Time a reception is done, according to the value 
  * of "ReceivedFrame" the examples runs a specified 
  * Routine :
  * If the reception is correct TransferStatus value is PASSED ,otherwise it is FAILED */
  
  while(1)
  {
    if(ReceivedFrame == 0)
    {
    }
    /* If Bad Reception  */
    if(ReceivedFrame == 2)
    {
      /* Not Ok */
      TransferStatus = FAILED;
      ReceivedFrame = 0;
    }
    if (ReceivedFrame == 1)
    {
      /* OK */
      if ((Tab_Rx[1] ==0x44) && (Tab_Rx[2] == 0x42)) /* Test on the opcode value */
      {
        TransferStatus = PASSED;
        ReceivedFrame = 0;
        CEC_FlushRxBuffer();
      }     
    }
  }
}      

/**
  * @brief  Configures the CEC peripheral.
  * @param  None
  * @retval None
  */
static void CEC_Config(void)
{
  CEC_InitTypeDef CEC_InitStruct;
  NVIC_InitTypeDef NVIC_InitStructure;
  GPIO_InitTypeDef GPIO_InitStructure;

  /* DeInitialize CEC to reinitialize from scratch */
  CEC_DeInit();
  
  /* Enable CEC clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_CEC, ENABLE);
  
  /* Enable CEC_LINE_GPIO clocks */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);

  /* Configure CEC_LINE_GPIO as Output open drain */
  GPIO_PinAFConfig(GPIOB, GPIO_PinSource6 , GPIO_AF3_CEC);
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
  GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_6 ;
  GPIO_Init(GPIOB, &GPIO_InitStructure);

  /* Enable the CEC global Interrupt (with higher priority) */
  NVIC_InitStructure.NVIC_IRQChannel = CEC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

  /* Configure CEC */
  CEC_InitStruct.CEC_SignalFreeTime     = CEC_SignalFreeTime_Standard;
  CEC_InitStruct.CEC_RxTolerance        = CEC_RxTolerance_Standard;
  CEC_InitStruct.CEC_StopReception      = CEC_StopReception_Off;
  CEC_InitStruct.CEC_BitRisingError     = CEC_BitRisingError_Off;
  CEC_InitStruct.CEC_LongBitPeriodError = CEC_LongBitPeriodError_Off;
  CEC_InitStruct.CEC_BRDNoGen           = CEC_BRDNoGen_On;
  CEC_InitStruct.CEC_SFTOption          = CEC_SFTOption_Off;
  CEC_Init(& CEC_InitStruct);
  
  /* Configure Own Address */
  CEC_OwnAddressConfig(MyOwnAddress);
  
  /* Enable Listen Mode */
  CEC_ListenModeCmd(ENABLE);
 
  /* Activate CEC interrupts associated to the set of TX flags */
  CEC_ITConfig(CEC_IT_TXEND|CEC_IT_TXBR,ENABLE);

  /* Activate CEC interrupts associated to the set of RX flags */
  CEC_ITConfig(CEC_IT_RXEND|CEC_IT_RXBR,ENABLE);

  /* Activate CEC interrupts associated to the set of TX error */
  CEC_ITConfig(CEC_IT_TXACKE|CEC_IT_TXERR|CEC_IT_ARBLST,ENABLE);

  /* Activate CEC interrupts associated to the set of RX error */
  CEC_ITConfig(CEC_IT_RXACKE|CEC_IT_LBPE|CEC_IT_SBPE|CEC_IT_BRE,ENABLE);

  /* Enable CEC */
  CEC_Cmd(ENABLE);
}

/**
  * @brief  Configures EXTI Line15 (connected to PG15 pin) in interrupt mode
  * @param  None
  * @retval None
  */
static void EXTILine13_15_Config(void)
{
  EXTI_InitTypeDef   EXTI_InitStructure;
  GPIO_InitTypeDef   GPIO_InitStructure;
  NVIC_InitTypeDef   NVIC_InitStructure;

  /* Enable GPIOC clock */
  RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE); 
  /* Enable SYSCFG clock */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
  
  /* Configure PG15 pin as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;
  GPIO_Init(GPIOC, &GPIO_InitStructure);

  /* Connect EXTI Line15 to PG15 pin */
  SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOC, EXTI_PinSource13);
  
  /* Configure EXTI Line15 */
  EXTI_InitStructure.EXTI_Line = EXTI_Line13;
  EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
  EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;
  EXTI_InitStructure.EXTI_LineCmd = ENABLE;
  EXTI_Init(&EXTI_InitStructure);

  /* Enable and set EXTI15_10 Interrupt to the lowest priority */
  NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

  NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  Reset CEC reception buffer
  * @param  None
  * @retval None
  */
static void CEC_FlushRxBuffer(void)
{
  uint32_t cpt;
  
  for (cpt = CEC_MAX_PAYLOAD; cpt > 0; cpt--)
  {
    Tab_Rx[cpt-1] = 0;
  }
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */ 

/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
