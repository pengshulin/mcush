/**
  ******************************************************************************
  * @file    FMC/FMC_SDRAM_DMA/main.c 
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

/** @addtogroup FMC_SDRAM_DMA
  * @{
  */
  
/* Private typedef -----------------------------------------------------------*/
/* Private defines -----------------------------------------------------------*/
#define BUFFER_SIZE              ((uint32_t)0x0100)
#define WRITE_READ_ADDR          ((uint32_t)0x0800)  
#define SDRAM_BANK_ADDRESS       ((uint32_t)0xC0000000) 

/* Maximum timeout value */
#define TIMEOUT_MAX              ((uint32_t)10000) 

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Read/Write Buffers */
uint32_t aDMA_BufferSRC32[BUFFER_SIZE];
uint32_t aDMA_BufferDST32[BUFFER_SIZE];

/* Status variables */
__IO uint32_t uwWriteReadStatus = 0;

/* Counter index */
uint32_t uwIndex = 0;

/* Memory address */
uint32_t uwDMA_FMC_MemoryAddress;

/* DMA transfer complete flag */
__IO uint32_t uwDMA_Transfer_Complete = 0;


/* Private function prototypes -----------------------------------------------*/
static void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset);
static void DMA_Config_WriteWord(void);
static void DMA_Config_ReadWord(void);


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
       files (startup_stm32f40_41xxx.s/startup_stm32f427_437xx.s/
       startup_stm32f429_439xx.s/startup_stm32f401xx.s) before to branch to 
       application main. To reconfigure the default setting of SystemInit() 
       function, refer to system_stm32f4xx.c file
     */       
     
  /* Initialize LEDs mounted on EVAL board */
  STM_EVAL_LEDInit(LED1);
  STM_EVAL_LEDInit(LED2); 
  STM_EVAL_LEDInit(LED3); 
  
  /* Initialize the SDRAM memory */
  SDRAM_Init();
  
  /* Fill the buffer to send */
  Fill_Buffer(aDMA_BufferSRC32, BUFFER_SIZE, 0x458F);
  
  /* Attribute the peripheral address */
  uwDMA_FMC_MemoryAddress = SDRAM_BANK_ADDRESS + WRITE_READ_ADDR; 
  
  /* Write data to the SDRAM memory with DMA transaction */
  DMA_Config_WriteWord();
  
  /* Read back data from the SDRAM memory with DMA transaction */
  DMA_Config_ReadWord();
  
  /* Check the SDRAM memory content correctness */   
  for (uwIndex = 0; (uwIndex < BUFFER_SIZE) && (uwWriteReadStatus == 0); uwIndex++)
  {
    if (aDMA_BufferDST32[uwIndex] != aDMA_BufferSRC32[uwIndex])
    {
      uwWriteReadStatus++;
    }
  }	

  if (uwWriteReadStatus)
  {
    /* KO */
    /* Turn on LED2 */
    STM_EVAL_LEDOn(LED2);  
  }
  else
  { 
    /* OK */
    /* Turn on LED1 */
    STM_EVAL_LEDOn(LED1);   
  }

  while (1)
  {
  }  
}

/**
  * @brief  Configures the DMA controller according to the Stream parameters
  *         defined in main.h file with word size 
  * @param  None
  * @retval None
  */
void DMA_Config_WriteWord(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  DMA_InitTypeDef  DMA_InitStructure;
  __IO uint32_t    timeout = TIMEOUT_MAX;
    
  /* Reset DMA transfer complete flag */
  uwDMA_Transfer_Complete = 0;
  
  /* Enable DMA clock */
  RCC_AHB1PeriphClockCmd(DMA_STREAM_CLOCK, ENABLE);
  
  /* Reset DMA Stream registers (for debug purpose) */
  DMA_DeInit(DMA_STREAM);

  /* Check if the DMA Stream is disabled before enabling it.
     Note that this step is useful when the same Stream is used multiple times:
     enabled, then disabled then reenabled... In this case, the DMA Stream disable
     will be effective only at the end of the ongoing data transfer and it will 
     not be possible to reconfigure it before making sure that the Enable bit 
     has been cleared by hardware. If the Stream is used only once, this step might 
     be bypassed. */
  while (DMA_GetCmdStatus(DMA_STREAM) != DISABLE)
  {
  }
  
  /* Enable the DMA Stream IRQ Channel */
  NVIC_InitStructure.NVIC_IRQChannel = DMA_STREAM_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure); 
  
  /* Configure DMA Stream */
  DMA_InitStructure.DMA_Channel = DMA_CHANNEL;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)aDMA_BufferSRC32;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)uwDMA_FMC_MemoryAddress;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;
  DMA_InitStructure.DMA_BufferSize = (uint32_t)BUFFER_SIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_INC4;
  DMA_Init(DMA_STREAM, &DMA_InitStructure);
    
  /* Enable DMA Stream Transfer Complete interrupt */
  DMA_ITConfig(DMA_STREAM, DMA_IT_TC, ENABLE);

  /* DMA Stream enable */
  DMA_Cmd(DMA_STREAM, ENABLE);

  /* Check if the DMA Stream has been effectively enabled.
     The DMA Stream Enable bit is cleared immediately by hardware if there is an 
     error in the configuration parameters and the transfer is no started (ie. when
     wrong FIFO threshold is configured ...) */
  timeout = TIMEOUT_MAX;
  while ((DMA_GetCmdStatus(DMA_STREAM) != ENABLE) && (timeout-- > 0))
  {
  }
   
  /* Check if a timeout condition occurred */
  if (timeout == 0)
  {
    /* Manage the error: to simplify the code enter an infinite loop */
    while (1)
    {
    }
  }
  
  /* Wait until DMA transfer is complete */
  while (!uwDMA_Transfer_Complete)
  {
  }
  
}

/**
  * @brief  Configure the DMA controller according to the Stream parameters
  *         defined in main.h file with word size
  * @param  None
  * @retval None
  */
void DMA_Config_ReadWord(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  DMA_InitTypeDef  DMA_InitStructure;
  __IO uint32_t    timeout = TIMEOUT_MAX;
    
  /* Reset DMA transfer complete flag */
  uwDMA_Transfer_Complete = 0;
  
  /* Enable DMA clock */
  RCC_AHB1PeriphClockCmd(DMA_STREAM_CLOCK, ENABLE);
  
  /* Reset DMA Stream registers (for debug purpose) */
  DMA_DeInit(DMA_STREAM);

  /* Check if the DMA Stream is disabled before enabling it.
     Note that this step is useful when the same Stream is used multiple times:
     enabled, then disabled then reenabled... In this case, the DMA Stream disable
     will be effective only at the end of the ongoing data transfer and it will 
     not be possible to reconfigure it before making sure that the Enable bit 
     has been cleared by hardware. If the Stream is used only once, this step might 
     be bypassed. */
  while (DMA_GetCmdStatus(DMA_STREAM) != DISABLE)
  {
  }
  
  /* Enable the DMA Stream IRQ Channel */
  NVIC_InitStructure.NVIC_IRQChannel = DMA_STREAM_IRQ;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);  
  
  /* Configure DMA Stream */
  DMA_InitStructure.DMA_Channel = DMA_CHANNEL;  
  DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)uwDMA_FMC_MemoryAddress;
  DMA_InitStructure.DMA_Memory0BaseAddr = (uint32_t)aDMA_BufferDST32;
  DMA_InitStructure.DMA_DIR = DMA_DIR_MemoryToMemory;
  DMA_InitStructure.DMA_BufferSize = (uint32_t)BUFFER_SIZE;
  DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Enable;
  DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Word;
  DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Word;
  DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
  DMA_InitStructure.DMA_Priority = DMA_Priority_High;
  DMA_InitStructure.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStructure.DMA_FIFOThreshold = DMA_FIFOThreshold_Full;
  DMA_InitStructure.DMA_MemoryBurst = DMA_MemoryBurst_INC4;
  DMA_InitStructure.DMA_PeripheralBurst = DMA_PeripheralBurst_INC4;
  DMA_Init(DMA_STREAM, &DMA_InitStructure);
    
  /* Enable DMA Stream Transfer Complete interrupt */
  DMA_ITConfig(DMA_STREAM, DMA_IT_TC, ENABLE);

  /* DMA Stream enable */
  DMA_Cmd(DMA_STREAM, ENABLE);

  /* Check if the DMA Stream has been effectively enabled.
     The DMA Stream Enable bit is cleared immediately by hardware if there is an 
     error in the configuration parameters and the transfer is no started (ie. when
     wrong FIFO threshold is configured ...) */
  timeout = TIMEOUT_MAX;
  while ((DMA_GetCmdStatus(DMA_STREAM) != ENABLE) && (timeout-- > 0))
  {
  }
   
  /* Check if a timeout condition occurred */
  if (timeout == 0)
  {
    /* Manage the error: to simplify the code enter an infinite loop */
    while (1)
    {
    }
  }

  /* Wait until DMA transfer is complete */
  while (!uwDMA_Transfer_Complete)
  {
  }  
  
}

/**
  * @brief  Fills buffer with user predefined data.
  * @param  pBuffer: pointer on the buffer to fill
  * @param  BufferLenght: size of the buffer to fill
  * @param  Offset: first value to fill on the buffer
  * @retval None
  */
void Fill_Buffer(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
{
  uint32_t tmpIndex = 0;

  /* Put in global buffer different values */
  for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++ )
  {
    pBuffer[tmpIndex] = tmpIndex + uwOffset;
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
