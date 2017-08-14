/**
  ******************************************************************************
  * @file    FMC/FMC_SDRAM_SRAM_BankSwitch/main.c 
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

/** @addtogroup FMC_SDRAM_BankSwitch
  * @{
  */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BUFFER_SIZE         ((uint32_t)0x0100)
#define WRITE_READ_ADDR     ((uint32_t)0x0800)

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Read/Write Buffers */
uint32_t aTxBuffer_32[BUFFER_SIZE];
uint32_t aRxBuffer_32[BUFFER_SIZE];
uint16_t aTxBuffer_16[BUFFER_SIZE];
uint16_t aRxBuffer_16[BUFFER_SIZE];

/* Status variables */
__IO uint32_t uwWriteReadStatus_SDRAM = 0;
__IO uint32_t uwWriteReadStatus_SRAM = 0;

/* Counter index */
uint32_t uwIndex = 0;

/* Global structures */
FMC_SDRAMCommandTypeDef FMC_SDRAMCommandStructure;

/* Private function prototypes -----------------------------------------------*/
static void Fill_Buffer32(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset);
static void Fill_Buffer16(uint16_t *pBuffer, uint32_t uwBufferLenght, uint16_t uhOffset);

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
  
  /* Initialize the SDRAM memory */
  SDRAM_Init();
  
  /* Initialize the SRAM memory */
  SRAM_Init();
  
  /* Fill the buffer to send */
  Fill_Buffer32(aTxBuffer_32, BUFFER_SIZE, 0x250F);
  
  /* Write data to the SDRAM memory */
  SDRAM_WriteBuffer(aTxBuffer_32, WRITE_READ_ADDR, BUFFER_SIZE); 
     
  /* Read back data from the SDRAM memory */
  SDRAM_ReadBuffer(aRxBuffer_32, WRITE_READ_ADDR, BUFFER_SIZE);  
  
  /* Check the SDRAM memory content correctness */   
  for (uwIndex = 0; (uwIndex < BUFFER_SIZE) && (uwWriteReadStatus_SDRAM == 0); uwIndex++)
  {
    if (aRxBuffer_32[uwIndex] != aTxBuffer_32[uwIndex])
    {
      uwWriteReadStatus_SDRAM++;
    }
  }	
  
  /* Configure a precharge all (PALL) command to put FMC SDRAM bank in idle state
     before switching to another bank */ 
  FMC_SDRAMCommandStructure.FMC_CommandMode = FMC_Command_Mode_PALL;
  FMC_SDRAMCommandStructure.FMC_CommandTarget = FMC_Command_Target_bank1;
  FMC_SDRAMCommandStructure.FMC_AutoRefreshNumber = 1;
  FMC_SDRAMCommandStructure.FMC_ModeRegisterDefinition = 0;
  
  /* Send the command */
  FMC_SDRAMCmdConfig(&FMC_SDRAMCommandStructure);
  
  /* Wait until the SDRAM controller is ready */ 
  while(FMC_GetFlagStatus(FMC_Bank1_SDRAM, FMC_FLAG_Busy) != RESET)
  {
  }
  
  /* Fill the buffer to send */
  Fill_Buffer16(aTxBuffer_16, BUFFER_SIZE, 0x250F);
  
  /* Write data to the SRAM memory */
  SRAM_WriteBuffer(aTxBuffer_16, WRITE_READ_ADDR, BUFFER_SIZE); 
  
  /* Read back data from the SRAM memory */
  SRAM_ReadBuffer(aRxBuffer_16, WRITE_READ_ADDR, BUFFER_SIZE);  
  
  /* Check the SRAM memory content correctness */   
  for (uwIndex = 0x00; (uwIndex < BUFFER_SIZE) && (uwWriteReadStatus_SRAM == 0); uwIndex++)
  {
    if (aRxBuffer_16[uwIndex] != aTxBuffer_16[uwIndex])
    {
      uwWriteReadStatus_SRAM++;
    }
  }
  
  if (uwWriteReadStatus_SRAM || uwWriteReadStatus_SDRAM)
  {
    /* KO */
    /* Turn on LD2 */
    STM_EVAL_LEDOn(LED2); 
  }
  else
  { 
    /* OK */
    /* Turn on LD1 */
    STM_EVAL_LEDOn(LED1);    
  }

  while (1)
  {
  } 
  
}

/**
  * @brief  Fills buffer with user predefined 32 bit length data.
  * @param  pBuffer: pointer on the buffer to fill
  * @param  uwBufferLenght: size of the buffer to fill
  * @param  uwOffset: first value to fill on the buffer
  * @retval None
  */
static void Fill_Buffer32(uint32_t *pBuffer, uint32_t uwBufferLenght, uint32_t uwOffset)
{
  uint32_t tmpIndex = 0;

  /* Put in global buffer different values */
  for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++ )
  {
    pBuffer[tmpIndex] = tmpIndex + uwOffset;
  }
}

/**
  * @brief  Fills buffer with user predefined 16 bit length data.
  * @param  pBuffer: pointer on the buffer to fill
  * @param  uwBufferLenght: size of the buffer to fill
  * @param  uhOffset: first value to fill on the buffer
  * @retval None
  */
static void Fill_Buffer16(uint16_t *pBuffer, uint32_t uwBufferLenght, uint16_t uhOffset)
{
  uint32_t tmpIndex = 0;

  /* Put in global buffer different values */
  for (tmpIndex = 0; tmpIndex < uwBufferLenght; tmpIndex++ )
  {
    pBuffer[tmpIndex] = tmpIndex + uhOffset;
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
